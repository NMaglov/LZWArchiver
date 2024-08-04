#include "Utils.h"
int main(int argc, char *argv[])
{
    // for (int i = 0; i < argc; i++)
    // {
    //     std::cout << i << " " << argv[i] << "\n";
    // }
    executeCommands(argc, argv);

    // Archive a;
    // if (argc < 3)
    // {
    //     std::cout << "Invalid operation" << std::endl;
    //     return 0;
    // }
    // if (strcmp(argv[1], "ZIP") == 0)
    // {
    //     std::string str = getCanonicalPath(std::filesystem::current_path().string() + std::filesystem::path::preferred_separator + argv[2]);
    //     std::cout << str << "\n";
    //     std::ofstream of(str);
    //     std::fstream where(str);

    //     std::vector<ZipEntry> zips;
    //     for (int i = 3; i < argc; i++)
    //     {
    //         std::vector<ZipEntry> cur = match(argv[i]);
    //         zips.insert(zips.end(), cur.begin(), cur.end());
    //     }
    //     std::vector<std::istream *> from;
    //     for (ZipEntry &z : zips)
    //     {
    //         std::cout << z.fullPath << "\n";
    //         std::ifstream *ifs = new std::ifstream(z.fullPath);
    //         from.push_back(ifs);
    //     }
    //     a.create(where, from, zips);
    //     for (int i = from.size() - 1; i >= 0; i--)
    //     {
    //         delete from[i];
    //     }
    // }
    // else if (strcmp(argv[1], "UNZIP") == 0)
    // {
    //     std::ifstream from(argv[3]);
    //     std::vector<std::string> what;
    //     for (int i = 4; i < argc; i++)
    //     {
    //         what.push_back(argv[i]);
    //     }
    //     std::string where = argv[2];
    //     std::cout << what.size() << "\n";
    //     a.unzip(where, from, what);
    // }
    // else if (strcmp(argv[1], "REFRESH") == 0)
    // {
    //     a.refresh(argv[2], argv[3]);
    // }
    // else if (strcmp(argv[1], "INFO") == 0)
    // {
    //     std::string str = getCanonicalPath(std::filesystem::current_path().string() + std::filesystem::path::preferred_separator + argv[2]);
    //     std::ifstream ifs(str);
    //     a.info(ifs, std::cout);
    // }
    // else if (strcmp(argv[1], "EC") == 0)
    // {
    //     std::string str = getCanonicalPath(std::filesystem::current_path().string() + std::filesystem::path::preferred_separator + argv[2]);
    //     std::ifstream ifs(str);
    //     a.checkDamaged(ifs, std::cout);
    // }
    // else
    // {
    //     std::cout << "Invalid operation" << std::endl;
    //     return 0;
    // }
    return 0;
}