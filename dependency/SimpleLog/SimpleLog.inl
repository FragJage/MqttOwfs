
template<>
inline SimpleLog::Line& SimpleLog::Line::operator<< <bool>(const bool &b)
{
	*m_message << (b ? "True" : "False");
	return *this;
}

template<>
inline SimpleLog::Line& SimpleLog::Line::operator<< <std::string>(const std::string &s)
{
	*m_message << "'" << s << "'";
	return *this;
}

template<class T>
inline SimpleLog::Line& SimpleLog::Line::operator<<(const std::set<T> &set) 
{
	bool first = true;

	*m_message << "set (";
	for (typename std::set<T>::const_iterator it = set.begin(); it != set.end(); ++it)
	{
		if (!first) *m_message << ", "; else first = false;
		*m_message << *it;
	}
	*m_message << ")";
	return *this;
}

template<class T>
inline SimpleLog::Line& SimpleLog::Line::operator<<(const std::multiset<T> &set)
{
	bool first = true;

	*m_message << "multiset (";
	for (typename std::multiset<T>::const_iterator it = set.begin(); it != set.end(); ++it)
	{
		if (!first) *m_message << ", "; else first = false;
		*m_message << *it;
	}
	*m_message << ")";
	return *this;
}

template<class T>
inline SimpleLog::Line& SimpleLog::Line::operator<<(const std::list<T> &obj)
{
	bool first = true;

	*m_message << "list (";
	for (typename std::list<T>::const_iterator it = obj.begin(); it != obj.end(); ++it)
	{
		if (!first) *m_message << ", "; else first = false;
		*m_message << *it;
	}
	*m_message << ")";
	return *this;
}

template<class T>
inline SimpleLog::Line& SimpleLog::Line::operator<<(const std::vector<T> &obj)
{
	bool first = true;

	*m_message << "vector (";
	for (typename std::vector<T>::const_iterator it = obj.begin(); it != obj.end(); ++it)
	{
		if (!first) *m_message << ", "; else first = false;
		*m_message << *it;
	}
	*m_message << ")";
	return *this;
}

template<class T>
inline SimpleLog::Line& SimpleLog::Line::operator<<(const std::deque<T> &obj)
{
	bool first = true;

	*m_message << "deque (";
	for (typename std::deque<T>::const_iterator it = obj.begin(); it != obj.end(); ++it)
	{
		if (!first) *m_message << ", "; else first = false;
		*m_message << *it;
	}
	*m_message << ")";
	return *this;
}

template<class T1, class T2>
inline SimpleLog::Line& SimpleLog::Line::operator<<(const std::pair<T1, T2> &pair)
{
	*this << "pair [" << pair.first << "]='" << pair.second << "'";
	return *this;
}

template<class T1, class T2>
inline SimpleLog::Line& SimpleLog::Line::operator<< (const std::map<T1, T2> &map)
{
	bool first = true;

	for (typename std::map<T1, T2>::const_iterator it = map.begin(); it != map.end(); ++it) 
	{
		if(!first)
			*m_message << ", ";
		else
			first = false;
		
		*m_message << "map[" << it->first << "]='" << it->second << "'";
	}
	return *this;
}

template<class T1, class T2>
inline SimpleLog::Line& SimpleLog::Line::operator<< (const std::multimap<T1, T2> &map)
{
	bool first = true;

	for (typename std::multimap<T1, T2>::const_iterator it = map.begin(); it != map.end(); ++it)
	{
		if (!first)
			*m_message << ", ";
		else
			first = false;

		*m_message << "multimap[" << it->first << "]='" << it->second << "'";
	}
	return *this;
}