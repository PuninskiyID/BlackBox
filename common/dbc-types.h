#ifndef _DBC_TYPES_H_
#define _DBC_TYPES_H_

#include <iostream>
#include <map>
#include <stdio.h>
#include <deque>
#include <list>
#include <vector>
#include <regex>

namespace Dbc
{
	class Parser;
	
	class Signal
	{
		friend class Dbc::Parser;
	protected:
		std::string name;
		unsigned int type; /*unsigned, signed*/
		unsigned int format; /*Intel, Motorola*/
		unsigned int pos;
		unsigned int len;
		double offset;
		double factor;
		double min;
		double max;
		std::string unit;
		std::vector<std::string> receiveNodes;
		std::map<unsigned int, std::string> values;
		std::string comment;
	public:
		double parseFromRaw(uint64_t raw) const;
		double parseFromCanData(uint64_t bits) const;

		const std::vector<std::string>& getReceiveNodes() const { return receiveNodes; }
		const std::map<unsigned int, std::string>& getValues() const { return values; }
		const std::string& getName() const { return name; }
		unsigned int getType() const { return type; }
		unsigned int getFormat() const { return format; }
		unsigned int getPos() const { return pos; }
		unsigned int getLen() const { return len; }
		double getOffset() const { return offset; }
		double getFactor() const { return factor; }
		double getMin() const { return min; }
		double getMax() const { return max; }
		const std::string& getUnit() const { return unit; }
		const std::string& getComment() const { return comment; }

		Signal(): name(""), type(0), format(0), pos(0), len(0), offset(0.0), factor(1.0), min(0), max(1.0), comment("") {  }
	};

	class Message
	{
		friend class Dbc::Parser;
	protected:
		std::string name;
		uint32_t id;
		unsigned int dataLength;
		std::string sendingNode;
		std::list<Dbc::Signal> _signals;
		std::map<std::string, Dbc::Signal*> signalByName;
		std::string comment;
	public:
	
		const std::string& getName() const { return name; }
		uint32_t getId() const { return id; }
		unsigned int getDataLength() const { return dataLength; }
		const std::string& getComment() const { return comment; }
		const std::list<Signal>& getSignals() const { return _signals; }
        bool containsSignal(const std::string& name) const { return signalByName.count(name); }
        const Signal& getSignal(const std::string& name) const { return *signalByName.at(name); }

		Message(): name(""), id(0), dataLength(0), sendingNode(""), _signals(), signalByName(), comment("") { }
	};

	struct Node
	{
		std::string name;
		std::string comment;
		Node() { name = ""; comment = ""; }
		Node(const std::string& name) { this->name = name; }
	};

	class Parser
	{
		std::regex re_msg;
		std::regex re_sgn;
		std::regex re_int;
        std::regex re_nodes;
		std::regex re_node_comment;
		std::regex re_msg_comment;
		std::regex re_sgn_comment;
		std::regex re_set_attr_cycle_time;
		std::regex re_set_attr_send_type;
		std::regex re_set_msg_enum;
		std::regex re_msg_enum_cell;
		std::map<std::string, Dbc::Message*> mapMsgByName;
		std::map<uint32_t, Dbc::Message*> mapMsgById;
		std::list<Dbc::Node> nodes;
		std::map<std::string, Dbc::Node*> mapNodeByName;
		std::list<Dbc::Message> messages;
	public:
		Parser();
		void openFile( const std::string& path );
		bool containsMessage(uint32_t id) const
		{
			return mapMsgById.count(id);
		}
		bool containsMessage(const std::string& name) const
		{
			return mapMsgByName.count(name);
		}
		const std::list<Message>& getMessages() const
		{
			return messages;
		}
		const Message& getMessage(const uint32_t id)
		{
			return *mapMsgById[id];
		}
		const Message& getMessage(const std::string&name)
		{
			return *mapMsgByName[name];
		}
		const std::list<Node>& getNodes() const
		{
			return nodes;
		}
		bool containsNode(const std::string& name) const
		{
			return mapNodeByName.count(name);
		}
		const Node& getNode(const std::string& name)
		{
			return *mapNodeByName[name];
		}
	};
}

#endif /*_DBC_TYPES_H_*/
