#include <boost/program_options.hpp>
#include <boost/crc.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_set>
#include <filesystem>
#include <algorithm>
#include <cctype>

namespace po = boost::program_options;

enum class HashAlgorithm {crc32, std};

void toLower(std::string& str) {
    std::for_each(str.begin(), str.end(), [](char& c) {c = std::tolower(c);});
}

std::unordered_set<std::filesystem::path> collectFiles(const std::filesystem::path& dir, const std::unordered_set<std::filesystem::path>& excludedDirs, const std::vector<std::string>& masks, int level, std::size_t minFileSize) {
    if (level < 0 || excludedDirs.count(dir)) {
        return {};
    }    
    
    std::unordered_set<std::filesystem::path> res;
    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        if (entry.is_directory()) {            
            const auto files = collectFiles(entry, excludedDirs, masks, level - 1, minFileSize);
            res.insert(files.begin(), files.end());
        } else if (entry.file_size() > minFileSize) {
            if (masks.empty()) {
                res.insert(entry.path());
            } else {
                std::string file(entry.path().filename().c_str());
                toLower(file);
                for (const auto& mask : masks) {
                    std::string maskLo = mask;
                    toLower(maskLo);
                    if (file.find(maskLo) != std::string::npos) {
                        res.insert(entry.path());
                        break;
                    }
                }
            }
        }
    }
    return res;
}


struct ParseInfo {
    ParseInfo(std::filesystem::path filePath, size_t fileSize) : path_(filePath), fileSize_(fileSize) {}

    const std::filesystem::path path_;
    const size_t fileSize_;
    std::vector<size_t> hashes_;
    bool isDuplicateFound_ = false;
    std::ifstream file_;
};

size_t calcHash(std::ifstream& file, size_t blockSize, HashAlgorithm algorithm) {
    std::string buf(blockSize, '\0');
    file.read(&buf[0], buf.size());

    size_t hash;
    if (algorithm == HashAlgorithm::std) {
        hash = std::hash<std::string>{}(buf);
    } else {
        boost::crc_32_type crc;
        crc.process_bytes(&buf[0], buf.size());
        hash = crc.checksum();
    }
    return hash;
}

bool isTwoFilesEqual(ParseInfo& info1, ParseInfo& info2, size_t blockSize, HashAlgorithm algorithm) {
    if (!info1.file_.is_open()) {
        info1.file_.open(info1.path_, std::ios_base::binary);
    }
    if (!info2.file_.is_open()) {
        info2.file_.open(info2.path_, std::ios_base::binary);
    }

    if (!info1.file_.is_open() || !info2.file_.is_open()) {
        return false;
    }

    const size_t minSize = std::min(info1.hashes_.size(), info2.hashes_.size());
    size_t blockNum = 0;
    while (blockNum < minSize) {
        if (info1.hashes_[blockNum] != info2.hashes_[blockNum]) {
            return false;
        }
        blockNum++;
    }
    
    while (info1.file_ && info1.hashes_.size() < info2.hashes_.size()) {
        info1.hashes_.push_back(calcHash(info1.file_, blockSize, algorithm));
        if (info1.hashes_[blockNum] != info2.hashes_[blockNum]) {
            return false;
        }
        blockNum++;
    }

    while (info2.file_ && info1.hashes_.size() > info2.hashes_.size()) {
        info2.hashes_.push_back(calcHash(info2.file_, blockSize, algorithm));
        if (info1.hashes_[blockNum] != info2.hashes_[blockNum]) {
            return false;
        }
        blockNum++;
    }

    while (info1.file_ && info2.file_) {
        info1.hashes_.push_back(calcHash(info1.file_, blockSize, algorithm));
        info2.hashes_.push_back(calcHash(info2.file_, blockSize, algorithm));
        if (info1.hashes_[blockNum] != info2.hashes_[blockNum]) {
            return false;
        }
        blockNum++;
    }  
    
    if (!info1.file_ && !info2.file_) {
        info1.isDuplicateFound_ = true;
        info2.isDuplicateFound_ = true;
        return true;
    }

    return false;
}

std::vector<std::vector<std::filesystem::path>> findDuplicates(const std::unordered_set<std::filesystem::path>& files, size_t blockSize, HashAlgorithm algorithm) {
    if (files.size() < 2) {
        return {};
    }
    
    std::vector<ParseInfo> info;
    info.reserve(files.size());
    for (auto it = files.cbegin(); it != files.cend(); ++it) {
        info.emplace_back(*it, std::filesystem::file_size(*it));
    }
    
    std::vector<std::vector<std::filesystem::path>> res;
    for (size_t i = 0; i < info.size() - 1; i++) {
        if (info[i].isDuplicateFound_) {
            continue;
        }

        std::vector<std::filesystem::path> group;
        for (size_t j = i + 1; j < info.size(); j++) {
            if (!info[j].isDuplicateFound_ && info[i].fileSize_ == info[j].fileSize_) {
                if (isTwoFilesEqual(info[i], info[j], blockSize, algorithm)) {
                    group.push_back(info[j].path_);
                }
            }
        }

        if (!group.empty()) {
            group.push_back(info[i].path_);
            res.emplace_back(std::move(group));
        }
    }
    return res;
}


int main(int argc, char* argv[]) {
    try {
        int level, fileMinSize, blockSize;
        std::string hashAlgorithm;

        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "produce help message")
            ("scan_dir",    po::value<std::vector<std::string>>()->multitoken(),            "scan directories")
            ("scan_dir_ex", po::value<std::vector<std::string>>()->multitoken(),            "excluded scan directories")
            ("level",       po::value<int>(&level)->default_value(0),                       "level scan")
            ("file_size",   po::value<int>(&fileMinSize)->default_value(1),                 "minimum file size")
            ("mask",        po::value<std::vector<std::string>>()->multitoken(),            "file names masks")
            ("S",           po::value<int>(&blockSize)->default_value(5),                   "block size")
            ("H",           po::value<std::string>(&hashAlgorithm)->default_value("std"),   "hash algorithm");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << desc << std::endl;
            return 1;
        }

        std::vector<std::string> scanDirs;
        std::unordered_set<std::filesystem::path> scanDirsEx;
        std::vector<std::string> masks;
        if (vm.count("scan_dir")) {
            scanDirs = vm["scan_dir"].as<std::vector<std::string>>();
        }
        if (vm.count("scan_dir_ex")) {            
            const auto& dirs = vm["scan_dir_ex"].as<std::vector<std::string>>();
            scanDirsEx.insert(dirs.begin(), dirs.end());

        }
        if (vm.count("mask")) {
            masks = vm["mask"].as<std::vector<std::string>>();
        }         

        std::unordered_set<std::filesystem::path> selectedFiles;
        for (const auto& dir : scanDirs) {
            const auto paths = collectFiles(dir, scanDirsEx, masks, level < 0 ? 0 : level, fileMinSize < 1 ? 1 : fileMinSize);
            selectedFiles.insert(paths.begin(), paths.end());
        }
        
        std::for_each(hashAlgorithm.begin(), hashAlgorithm.end(), [](char& c) {c = std::tolower(c);});
        const auto algorithm = hashAlgorithm == "std" ? HashAlgorithm::std : HashAlgorithm::crc32;

        const auto duplicates = findDuplicates(selectedFiles, blockSize, algorithm);
        for (auto it = duplicates.begin(); it != duplicates.end(); ++it) {
            for (const auto& item : *it) {
                std::cout << item << "\n";
            }
            if (std::next(it, 1) == duplicates.end()) {
                std::cout.flush();
            } else {
                std::cout << "\n";
            }
        }
    } catch (const po::error& err) {
        std::cout << "Exception: " << err.what() << std::endl;
    }    
}