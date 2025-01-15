#ifndef _CAN_TRACE_H_
#define _CAN_TRACE_H_


#include <stdint.h>
#include <string>
#include <map>
#include <deque>
#include <list>

namespace CanTrace
{
	struct Message
	{
		double time;
		uint32_t id;
		unsigned int dataLength;
		uint64_t data;
	};
    
    class Parser;

	class Channel
	{
        friend class Parser;
    protected:
        std::list<std::deque<Message>> messages;
		std::map<uint32_t, std::deque<Message>*> mapMessagesById;
    public:
        Channel() { }
        bool containsMessages(uint32_t id) const
        {
            return mapMessagesById.count(id);
        }
        const std::deque<Message>& getMessages(uint32_t id) const
        {
            return *mapMessagesById.at(id);
        }
	};

	class Parser
	{
		Channel channels[4];
    public:
		Parser();
        bool containsChannel(unsigned int ch) const
        {
            return ch < 4;
        }
        const Channel& getChannel(unsigned int ch) const
        {
            return channels[ch];
        }
		void openFile(const std::string& path);
	};
}

#endif /*_CAN_TRACE_H_*/







































