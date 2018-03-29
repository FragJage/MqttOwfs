#include "StringTools.h"
#include <algorithm>
#include <functional>

using namespace std;

string& StringTools::ltrim(string& s)
{
    s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));
    return s;
}

// trim from end
string& StringTools::rtrim(string& s)
{
    s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());
    return s;
}

// trim from both ends
string& StringTools::trim(string& s)
{
    return ltrim(rtrim(s));
}

bool StringTools::Split(string const& source, char const delimiter, string* key, string* value)
{
	size_t pos;

	pos = source.find(delimiter);
	if(pos==string::npos) return false;

    *key = source.substr(0, pos);
    *key = StringTools::rtrim(*key);
    *value = source.substr(pos+1);
    *value = StringTools::ltrim(*value);

    return true;
}

void StringTools::ParasitCar(string& str)
{
    size_t fin=str.size();

    if(fin<1) return;

    if(str.at(fin-1)<' ') str.erase(fin-1);
}

string StringTools::ReplaceAll(string& strExpression, const string& strFind, const string& strReplace)
{
    size_t len;
    size_t pos;

    len = strFind.length();

	while((pos=strExpression.find(strFind)) != string::npos)
        strExpression.replace(pos, len, strReplace);

    return strExpression;
}

bool StringTools::IsEqualCaseInsensitive(string const& a, string const& b)
{
    unsigned int sz = a.size();
    if (b.size() != sz)
        return false;
    for (unsigned int i = 0; i < sz; ++i)
        if (tolower(a[i]) != tolower(b[i]))
            return false;
    return true;
}

string& StringTools::ToLower(string& data)
{
    std::transform(data.begin(), data.end(), data.begin(), ::tolower);
    return data;
}

string& StringTools::ToUpper(string& data)
{
    std::transform(data.begin(), data.end(), data.begin(), ::toupper);
    return data;
}

bool StringTools::IsNumber(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while ( (it != s.end()) && (isdigit(*it)||(*it=='.')||(*it=='-')||(*it=='+')) ) ++it;
    return !s.empty() && it == s.end();
}