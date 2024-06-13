#include <boost/program_options.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <filesystem>

namespace po = boost::program_options;

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
                for (const auto& mask : masks) {
                    if (file.find(mask) != std::string::npos) {
                        res.insert(entry.path());
                        break;
                    }
                }
            }
        }
    }
    return res;
}

int main(int argc, char* argv[]) {
    try {
        int level, fileMinSize, blockSize = -1;
        std::string hashAlgorithm;

        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "produce help message")
            ("scan_dir",    po::value<std::vector<std::string>>()->multitoken(),  "scan directories")
            ("scan_dir_ex", po::value<std::vector<std::string>>()->multitoken(),  "excluded scan directories")
            ("level",       po::value<int>(&level)->default_value(0),             "level scan")
            ("file_size",   po::value<int>(&fileMinSize)->default_value(1),       "minimum file size")
            ("mask",        po::value<std::vector<std::string>>()->multitoken(),  "file names masks")
            ("S",           po::value<int>(&blockSize),                           "block size")
            ("H",           po::value<std::string>(&hashAlgorithm),               "hash algorithm");

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

        std::unordered_set<std::filesystem::path> files;
        for (const auto& dir : scanDirs) {
            const auto paths = collectFiles(dir, scanDirsEx, masks, level < 0 ? 0 : level, fileMinSize < 1 ? 1 : fileMinSize);
            files.insert(paths.begin(), paths.end());
        }




        //test
        for (const auto &item : files) {
            std::cout << item << std::endl;
        }

    } catch (const po::error& err) {
        std::cout << "Exception: " << err.what() << std::endl;
    }    
}