#include "dbc-types.h"
#include <regex>
#include <fstream>
#include <math.h>
#include "utils.h"

double Dbc::Signal::parseFromRaw(uint64_t raw) const
{
	double val;
	switch(format)
	{
	case 0: /*Intel*/
		break;
	case 1: /*Motorola*/
		raw = Bits::reverseBits(raw, len);
		break;
	default:
		return NAN;
	}
	switch(type)
	{
	case 0: /*Unsigned*/
		val = (double)raw;
		break;
	case 1: /*Signed*/
		val = (double)Bits::bitsToSigned(raw, len);
		break;
	default:
		return NAN;
	}
//	return (val + offset) * factor;
    return val * factor + offset;
}

double Dbc::Signal::parseFromCanData(uint64_t bits) const
{
	bits = (bits >> pos) & (0xFFFFFFFFFFFFFFFF >> (64 - len));	
	return parseFromRaw(bits);
}

Dbc::Parser::Parser()
{
	messages.clear();

	re_msg = std::regex(
		"^BO_\\s+"
		"(\\d+)\\s+"		/*can id*/
		"([\\w_]+):\\s+"	/*name*/
		"(\\d+)\\s+"		/*len*/
		"([\\w_]+)\\s*$"	/*sender*/
	);

	re_sgn = std::regex(
		"^\\s+SG_\\s+"
		"([\\w_]+)\\s+:\\s" 					/*name*/
		"(\\d+)\\|"								/*bit pos*/
		"(\\d+)@"								/*bit len*/
		"([01])"								/*byte order*/
		"([\\+\\-])\\s+\\("						/*signed or not*/
		"([\\d\\.\\+\\-]+),"					/*scale*/
		"([\\d\\.\\+\\-]+)\\)\\s+\\["			/*offset*/
		"([\\d\\.\\+\\-]+)\\|"					/*min*/
		"([\\d\\.\\+\\-]+)\\]\\s\""				/*max*/
		"([^\"]*)\"\\s+"						/*unit*/
		"([\\w_]+(?:\\s*,\\s*[\\w_]+)*?)\\s*$"	/*receivers*/
	);

	re_int = std::regex(
		"^"
		"([\\+\\-]?\\d+|[\\+\\-]?\\d+.0*)" /*integer value*/
		"$"
	);

	re_node_comment = std::regex(
		"^\\s*CM_\\s+BU_\\s+"
		"([\\w_]+)\\s+\""			/*node name*/
		"([^\"]*)\"\\s*;\\s*$"		/*node comment*/
	);

	re_msg_comment = std::regex(
		"^\\s*CM_\\s+BO_\\s+"
		"(\\d+)\\s+\""				/*msg id*/
		"([^\"]*)\"\\s*;\\s*$"		/*msg comment*/
	);

	re_sgn_comment = std::regex(
		"^\\s*CM_\\s+SG_\\s+"
		"(\\d+)\\s+"				/*msg id*/
		"([\\w_]+)\\s+\""			/*sgn name*/
		"([^\"]*)\";"				/*sgn comment*/
	);

	re_set_attr_cycle_time = std::regex(
		"^\\s*BA_\\s+\"GenMsgCycleTime\"\\s+BO_\\s+"
		"([\\d]+)\\s+"									/*msg id*/
		"(\\d+)\\s*;\\s*$"								/*value*/
	);

	re_set_attr_send_type = std::regex(
		"^\\s*BA_\\s+\"Gendata.msendType\"\\s+BO_\\s+"
		"([\\d]+)\\s+"									/*msg id*/
		"(\\d+)\\s*;\\s*$"								/*value*/
	);

	re_set_msg_enum = std::regex(
		"^\\s*VAL_\\s+"
		"(\\d+)\\s+" /*id*/
		"([\\w_]+)\\s+" /*signal name*/
		"((?:\\d+\\s+\"[^\"]+\"\\s+)*(?:\\d+\\s+\"[^\"]+\"\\s*)?);\\s*$"
		//"(\\d+\\s+\\\"[\\w_]+\\\"\\s+)+;\\s*$"
	);

	re_msg_enum_cell = std::regex(
		"^"
		"(\\d+)\\s+\"" /*value*/
		"([^\"]*)\"\\s+"	/*name*/
		"(.*)$"				/*others*/
	);
		
	re_nodes = std::regex(
		"^BU_:\\s*"
		"((?:[\\w\\d_]+)?(\\s+(?:[\\w\\d_]+)))\\s*"
		"$"
	);
}

void Dbc::Parser::openFile( const std::string& path )
{
	mapMsgById.clear();
	mapMsgByName.clear();
	mapNodeByName.clear();
	messages.clear();
	nodes.clear();

	std::ifstream file;
	char *fileData = 0;
	unsigned int line;
	auto sms = std::smatch();
	std::string lastMessageName = "";

	file.open(path, std::ios_base::out | std::fstream::binary);
	if(!file.is_open()) throw "File open error";

	file.seekg(0, std::ios::end);
	size_t fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	fileData = new char[fileSize + 1];
	file.read(fileData, fileSize);
	fileData[fileSize - 1] = 0;
	std::deque<char*> lines;

	str::ReadLines(lines, fileData, fileSize);

	for(line = 0; line < lines.size(); ++line)
	{
		const std::string& _line = lines[line];
		if(regex_search(_line, sms, re_msg)) {
			if(sms.size() != 5) continue;
			Message msg;
			sscanf(sms[1].str().c_str(), "%u", &msg.id);
			msg.name = sms[2].str().c_str();
			sscanf(sms[3].str().c_str(), "%d", &msg.dataLength);
			msg.sendingNode = sms[4].str().c_str();
			messages.push_back(msg);
			mapMsgByName.insert(std::pair(messages.back().name, &messages.back()));
			mapMsgById.insert(std::pair(messages.back().id, &messages.back()));

			lastMessageName = msg.name;
		}
		else if(regex_search(_line, sms, re_nodes))
		{
			auto nodesNames = str::split(sms[1], ' ');
			for(auto &name : nodesNames)
			{
				nodes.push_back(Node(name));
				mapNodeByName.insert( std::pair(nodes.back().name, &nodes.back()) );
			}
		} 
		else if(regex_search(_line, sms, re_sgn))
		{
			Signal sgn;
			char buf[64];
			sscanf(sms[1].str().c_str(), "%s", buf);
			sgn.name = buf;
			sscanf(sms[2].str().c_str(), "%d", &sgn.pos);
			sscanf(sms[3].str().c_str(), "%d", &sgn.len);
			unsigned int format;
			sscanf(sms[4].str().c_str(), "%u", &format);
			switch(format)
			{
			case 0: sgn.format = 1; break;
			case 1: sgn.format = 0; break;
			default: 
				sgn.format = 0; break;
			}
			char type;
			sscanf(sms[5].str().c_str(), "%c", &type);
			switch(type)
			{
			case '+': sgn.type = 0; break;
			case '-': sgn.type = 1; break;
			default:
				sgn.type = 0; break;
			}
			sscanf(sms[6].str().c_str(), "%lf", &sgn.factor);
			sscanf(sms[7].str().c_str(), "%lf", &sgn.offset);
			sscanf(sms[8].str().c_str(), "%lf", &sgn.min);
			sscanf(sms[9].str().c_str(), "%lf", &sgn.max);
			sgn.unit = sms[10];			  
			sgn.receiveNodes = str::split(sms[11].str(), ',');
			mapMsgByName[lastMessageName]->_signals.push_back(sgn);
			mapMsgByName[lastMessageName]->signalByName.insert(std::pair(
				sgn.name, 
				&mapMsgByName[lastMessageName]->_signals.back()
			));
		}

		else if(regex_search(_line, sms, re_node_comment)) {
			std::string name = sms[1].str();
			std::string comment = sms[2].str();
			if(!mapNodeByName.count(name)) continue;
			mapNodeByName[name]->comment = comment;
		}
		else if(regex_search(_line, sms, re_msg_comment)) {
			uint32_t id;
			sscanf(sms[1].str().c_str(), "%u", &id);
			std::string comment = sms[2].str();
			mapMsgById[id]->comment = comment;
		}
		else if(regex_search(_line, sms, re_sgn_comment)) {
			uint32_t id;
			sscanf(sms[1].str().c_str(), "%u", &id);
			mapMsgById[id]->signalByName[sms[2].str()]->comment = sms[3].str();
		}
		else if(regex_search(_line, sms, re_set_msg_enum)) 
		{
			uint32_t id;
			sscanf(sms[1].str().c_str(), "%u", &id);
			std::string name = sms[2].str();
			std::string str_values = sms[3].str();

			if(!mapMsgById.count(id) || !mapMsgById[id]->signalByName.count(name)) continue;

			Signal& sng = *mapMsgById[id]->signalByName[name];

			while(regex_search(str_values, sms, re_msg_enum_cell))
			{
				unsigned int index;
				sscanf(sms[1].str().c_str(), "%u", &index);
				sng.values.insert(std::pair(index, sms[2].str()));
				str_values = sms[3].str();
			}
		}

	}
	
	delete[] fileData;
}
