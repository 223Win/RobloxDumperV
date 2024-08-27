#include "Encoding.h"

namespace {
    // Char set
    // we basically just shuffle the strings around :)
    char CharSet1[] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
    char CharSet2[] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
    int CharSet3[] = { 0, 18, 10, 20, 16, 7, 21, 2, 24, 12, 3, 9, 17, 22, 4, 8, 13, 5, 14, 25, 11, 1, 6, 23, 19, 15 };
    int CharSet4[] = { 20, 23, 17, 10, 8, 12, 13, 22, 6, 21, 24, 0, 14, 18, 3, 15, 7, 2, 25, 5, 19, 4, 16, 11, 9, 1 };

    int GetIndex(const char* CSet,char Target) {
        for (int i = 0; i <= 25; i++) {
            if (CSet[i] == Target) {
                return i;
            }
        }
        return -1;
    }
}



std::string ExUtils::Encoding::DecodeString(std::string EncodedVal)
{
    std::string Decrypted = "";
    for (auto Child : EncodedVal) {
        bool Upper = std::isupper(Child);
        if (Upper) {
            int Index = GetIndex(CharSet2, Child);
            bool IsFound = false;
            for (int i = 0; i <= 25; i++) {
                if (CharSet3[i] == Index) {
                    Decrypted.push_back(CharSet2[i]);
                    IsFound = true;
                }
            }

            if (!IsFound) {
                Decrypted.push_back(static_cast<char>(static_cast<int>(Child) - 5));
            }
        }
        else {
            int Index = GetIndex(CharSet1, Child);
            bool IsFound = false;
            for (int i = 0; i <= 25; i++) {
                if (CharSet4[i] == Index) {
                    Decrypted.push_back(CharSet1[i]);
                    IsFound = true;
                }
            }

            if (!IsFound) {
                Decrypted.push_back(static_cast<char>(static_cast<int>(Child) - 5));
            }
        }
    }
    return Decrypted;
}

std::string ExUtils::Encoding::EncodeString(std::string BaseString)
{
    std::string NewString = "";
    for (auto Child : BaseString) {
        bool Upper = std::isupper(Child);
        if (Upper) {
            bool FoundYet = false;
            for (int i = 0; i <= 25; i++) {
                if (CharSet2[i] == Child) {
                    NewString.push_back(CharSet2[CharSet3[i]]);
                    FoundYet = true;
                }
            }

            if (!FoundYet) {
                
                NewString.push_back(static_cast<char>(static_cast<int>(Child)+5));
            }

            FoundYet = false;
        }
        else {
            bool FoundYet = false;
            for (int i = 0; i <= 25; i++) {
                if (CharSet1[i] == Child) {
                    NewString.push_back(CharSet1[CharSet4[i]]);
                    FoundYet = true;
                }
            }

            if (!FoundYet) {
                NewString.push_back(static_cast<char>(static_cast<int>(Child) + 5));
            }

            FoundYet = false;
        }
    }

    return NewString;
}
