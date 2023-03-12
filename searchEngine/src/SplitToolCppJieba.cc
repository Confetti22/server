#include "../include/SplitToolCppJieba.h"

    

//cut函数
vector<string> SplitToolCppJieba::cut(const string &sentence)
{
    vector<string> words;
    _jieba.CutForSearch(sentence,words);
    for (auto it = words.begin(); it != words.end(); )
    {
        if (stopWords.find(*it) != stopWords.end())
        {
            it = words.erase(it);
        }
        else if ( *it == " " || *it == "\n" )
        {
            it = words.erase(it);
        }
        else
        {
            ++it;
        }
    }
    return words;
}


