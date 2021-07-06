/* decoder for bitmap encoded key event fields starting with "B: KEY" at /proc/bus/input/devices 
* to get keys associated with key events from 'linux/input-event-codes.h'
*/

#include <iostream>
#include <string>
#include <bitset>
#include <vector>
#include <sstream>
#include <algorithm>

class bitmapDecoder
{
public:
    bitmapDecoder()
    {
    }

    ~bitmapDecoder()
    {
    }

    void decode_hex_string(std::string &&hexstr)
    {
        std::string temp = std::move(hexstr);

        // std::cout << "temp: " << temp << "\n";
        int pos = 0;

        while (pos != -1)
        {
            std::string hexfield;
            int val;
            pos = temp.find(" ");
            hexfield = temp.substr(0, pos);
            std::stringstream ss(hexfield);
            ss.flags(std::ios_base::hex);
            ss >> val;
            std::bitset<64> bits(val);
            binVec.push_back(bits);
            temp = temp.substr(pos + 1, temp.length());
        }

        bitset_to_string();
        generate_key_codes();

        std::cout << "decoded values::" << std::endl;
        for (auto &i : keyCodes)
        {
            std::cout << std::hex << "0x" << i << '\t';
        }
    }

private:
    void bitset_to_string()
    {
        if (!binVec.empty())
        {
            for (auto &i : binVec)
            {
                strBits += i.to_string();
            }
            std::reverse(strBits.begin(), strBits.end());
        }
        else
        {
            std::cerr << "binVec is empty" << std::endl;
        }
    }

    void generate_key_codes()
    {
        if (!strBits.empty())
        {
            for (int i = 0; i < strBits.length(); i++)
            {
                if (strBits[i] == '1')
                {
                    keyCodes.push_back(i);
                }
            }
        }
        else
        {
            std::cerr << "strBits is empty" << std::endl;
        }
    }

    std::vector<std::bitset<64>> binVec;
    std::string strBits;
    std::vector<unsigned int> keyCodes;
};

int main()
{
    bitmapDecoder decoder;
    decoder.decode_hex_string("1000001 1 0 0 0 0 0 0 0 0 0");
}