#ifndef __SPLITTOOL_H__
#define __SPLITTOOL_H__
#include <memory>
#include <vector>
#include <string>
using std::string;
using std::vector;
class SplitTool
{
public:
    SplitTool()
    {

    }
    //virtual ~SplitTool();
    ~SplitTool()
    {
        
    }
    virtual vector<string> cut(const string& sentence)=0;
private:
    /* data */
};



#endif
