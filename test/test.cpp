#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "Utils.h"
#include "Archive.h"
#include <filesystem>

TEST_CASE("Pattern checking")
{
    REQUIRE(check("", ""));
    REQUIRE(check("", "****"));
    REQUIRE(check("a", "a*"));
    REQUIRE(check("a", "a*****"));
    REQUIRE(check("a", "*****a"));
    REQUIRE(check("", "?") == false);
    REQUIRE(check("", "w") == false);
    REQUIRE(check("abc", "abc"));
    REQUIRE(check("a", "a"));
    REQUIRE(check("abc", "*"));
    REQUIRE(check("abc", "*****"));
    REQUIRE(check("abc", "?b?"));
    REQUIRE(check("abc", "???"));
    REQUIRE(check("abcdef", "?b*"));
    REQUIRE(check("abcdef", "*e*"));
    REQUIRE(check("abcdef", "*f"));
    REQUIRE(check("abcdef", "?*"));
    REQUIRE(check("abcdef", "*b") == false);
    REQUIRE(check("abc", "*b") == false);
    REQUIRE(check("abcdef", "*z*") == false);
    REQUIRE(check("abcdef", "b*") == false);
    REQUIRE(check("abcdef", "**************************************************************************"));
}

TEST_CASE("Partial pattern checking")
{
    REQUIRE(partialCheck("", ""));
    REQUIRE(partialCheck("", "*"));
    REQUIRE(partialCheck("", "?"));
    REQUIRE(partialCheck("abc", "abcd"));
    REQUIRE(partialCheck("abc", "*z"));
    REQUIRE(partialCheck("abc", "?bc**"));
    REQUIRE(partialCheck("abc", "???q"));
    REQUIRE(partialCheck("abc", "abc*c"));
    REQUIRE(partialCheck("abc", "abd") == false);
    REQUIRE(partialCheck("abc", "adc*c") == false);
}

TEST_CASE("Path canonization")
{
    char separator = std::filesystem::path::preferred_separator;
    std::string sep;
    sep += separator;
    std::string path = sep + "home" + sep + "..";
    REQUIRE(getCanonicalPath(path) == "");
    path = sep + "home" + sep + "." + sep + "a";
    REQUIRE(getCanonicalPath(path) == (sep + "home" + sep + "a"));
    path = sep + "home" + sep + ".." + sep + "a" + sep + ".." + sep + "b" + sep + ".";
    REQUIRE(getCanonicalPath(path) == sep + "b");
}

TEST_CASE("CRC calculation")
{
    SECTION("no data")
    {
        std::stringstream ss;
        REQUIRE(calculateCRC(ss) == 0);
    }
    SECTION("single byte")
    {
        std::stringstream ss("a");
        int crc = calculateCRC(ss, 2, 3, 11);
        REQUIRE(crc == 5);
    }
    SECTION("longer data")
    {
        std::stringstream ss("1z2a3yumn1rr2");
        int crc = calculateCRC(ss, ss.str().size(), 8, 0x07);
        REQUIRE(crc == 226);
    }
}

TEST_CASE("unzip(zip(data)) == data")
{
    SECTION("empty data")
    {
        std::string data = "";
        std::stringstream in{data}, out;
        std::stringstream decompressed;
        LZWArchiver c;
        c.zip(in, out);
        c.unzip(out, decompressed, out.str().size());
        REQUIRE(data == decompressed.str());
    }
    SECTION("single byte")
    {
        std::string data = "z";
        std::stringstream in{data}, out;
        std::stringstream decompressed;
        LZWArchiver c;
        c.zip(in, out);
        c.unzip(out, decompressed, out.str().size());
        REQUIRE(data == decompressed.str());
    }
    SECTION("long random data")
    {
        std::string data = "abasftqwhtqwhetkwjhobzkvjhaofupwerqiy14989agalnsgai8e9ahiu01923hhalsdfiupsadfu009fspadfja;sdjflaskfdjlaskdfj;askdjf;asdjffnfj;asfja;soifjasp0=-/,sa'foasf-123jfa/sldjfa[sdfiu-as09fu2385-1]]faj;sdoifj0ghi1090";
        std::stringstream in{data}, out;
        std::stringstream decompressed;
        LZWArchiver c;
        c.zip(in, out);
        c.unzip(out, decompressed, out.str().size());
        REQUIRE(data == decompressed.str());
    }
    SECTION("another long random data")
    {
        std::string data = "aklsjdfpoasidfupsfo8uowa8e7f90asd8f9sajfjkshfpwy8hpaihga[w09ujg;hnbhlashfipw8euqpw8ytohjlnoip13u09109su[ashgjbgrlba]gagopahgypew98us8hgiasfuaps8fusa98fuaef8us;fnspofhpbua-89fisfpsudf0sdfsafaksjfsfisioafusaofuosfoasiufoisaufoasiufiosjnqeopq1320981023819023lnfasljop09saf89";
        std::stringstream in{data}, out;
        std::stringstream decompressed;
        LZWArchiver c;
        c.zip(in, out);
        c.unzip(out, decompressed, out.str().size());
        REQUIRE(data == decompressed.str());
    }
    SECTION("long data with patterns")
    {
        std::string data = "aaajjjjjjajajjajajajjaj12312312313lllll123llllajfkkkkklllkklkllllll123123321321llllpopopopopopopopopo";
        std::stringstream in{data}, out;
        std::stringstream decompressed;
        LZWArchiver c;
        c.zip(in, out);
        c.unzip(out, decompressed, out.str().size());
        REQUIRE(data == decompressed.str());
    }
}

TEST_CASE("Create archive")
{

    SECTION("only files")
    {
        std::vector<std::istream *> from;
        std::stringstream in1{"abc"};
        std::stringstream in2{"abbbbbb"};
        from.push_back(&in1);
        from.push_back(&in2);
        std::vector<ZipEntry> zips;
        zips.push_back({"in1", "home/in1", false, (int)in1.str().size()});
        zips.push_back({"in2", "home/in2", false, (int)in2.str().size()});
        Archive a;

        std::stringstream where;
        a.create(where, from, zips);
        std::stringstream ss;
        a.info(where, ss);
        std::string line;
        std::getline(ss, line);
        REQUIRE(line.substr(0, 3) == "in2");
        std::getline(ss, line);
        REQUIRE(line.substr(0, 3) == "in1");
    }
    SECTION("file and folder")
    {
        std::vector<std::istream *> from;
        std::stringstream in1{"abc"};
        std::stringstream in2{"abbbbbb"};
        from.push_back(&in1);
        from.push_back(&in2);
        std::vector<ZipEntry> zips;
        zips.push_back({"in1", "home/in1", true, (int)in1.str().size()});
        zips.push_back({"in2", "home/in2", false, (int)in2.str().size()});
        Archive a;

        std::stringstream where;
        a.create(where, from, zips);
        std::stringstream ss;
        a.info(where, ss);
        std::string line;
        std::getline(ss, line);
        REQUIRE(line.substr(0, 3) == "in2");
        std::getline(ss, line);
        REQUIRE(line == "");
    }
}

TEST_CASE("Check archived data changed")
{
    std::vector<std::istream *> from;
    std::stringstream in1{"abc"};
    std::stringstream in2{"abbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"};
    from.push_back(&in1);
    from.push_back(&in2);
    std::vector<ZipEntry> zips;
    zips.push_back({"in1", "home/in1", false, (int)in1.str().size()});
    zips.push_back({"in2", "home/in2", false, (int)in2.str().size()});
    Archive a;

    std::stringstream where;
    a.create(where, from, zips);

    std::stringstream ss;
    std::stringstream damaged{where.str()};
    damaged.seekp(-10, std::ios::end);
    damaged.put('e');
    a.checkDamaged(damaged, ss);
    REQUIRE(ss.str() == "Bad CRC in file: in2\n");
}