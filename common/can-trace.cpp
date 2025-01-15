#include "can-trace.h"
#include <fstream>
#include <QDebug>
#include "utils.h"

CanTrace::Parser::Parser()
{

}

void CanTrace::Parser::openFile(const std::string& path)
{
	std::ifstream file;
	char *fileData = 0;
	double s, us, time = 0.0, prevs = 0.0;
    unsigned int ch, bits, bitsPos, flags, len, arr[8];
	uint32_t id;
	uint64_t data;
	char *ptr;
	Message m;

	for(unsigned int i = 0; i < (sizeof(channels) / sizeof(channels[0])); ++i)
	{
		channels[i].mapMessagesById.clear();
	}

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
	lines.pop_front(); /*Ignore Head*/
	lines.pop_front(); /*Ignore Local time*/

	for(unsigned int i = 0; i < lines.size(); ++i)
	{
        try
        {
            const char* line = lines[i];
            ptr = lines[i]; /*sec begin*/
//            if( !std::isdigit(*ptr) || !std::isdigit(*(ptr + 1)) ) continue;

//            s = std::stod(ptr);
//            while(*ptr != ',') { ptr++; }
//            ptr++; /*usec begin*/
//            us = std::stod(ptr);
//            while(*ptr != '\t') { ptr++; }
//            ptr++; /*channel begin*/
//            ch = std::stoul(ptr);
//            if(ch > 3) continue;
//            while(*ptr != '\t') { ptr++; }
//            ptr++; /*ignore this field*/
//            flags = std::stoul(ptr, 0, 16);
//            while(*ptr != '\t') { ptr++; }
//            ptr++; /*id begin*/
//            id = std::stol(ptr, 0, 16);
//            if(flags & 1) id |= 0x80000000;
//            while(*ptr != '\t') { ptr++; }
//            ptr++; /*data length begin*/
//            m.dataLength = std::stoul(ptr);
//            while(*ptr != '\t') { ptr++; }
//            data = 0x0ULL;
//            bitsPos = 0;
//            for(;;)
//            {
//                ptr++; /*data begin*/
//                bits = std::stoul(ptr, 0, 16);
//                data |= ((uint64_t)bits & 0xffULL) << bitsPos;
//                bitsPos += 8;
//                while(*ptr != ' ' && *ptr != '\t') { ptr++; }
//                if(*ptr == '\t') break;
//            }

            if(sscanf(ptr, "%2lf,%6lf\t%d\t%8x\t", &s, &us, &ch, &flags) != 4)
            {
                continue;
            }
            ptr += 21;
            if(sscanf(ptr, (flags & 1)? "%8x\t%d" : "%3x\t%d", &id, &len) != 2)
            {
                continue;
            }
            if(flags & 1)
            {
                id |= 0x80000000U;
            }
            ptr += (flags & 1)? 11 : 6;
            if(len > 8)
            {
                continue;
            }
            if(sscanf(ptr, (const char*)"%2x %2x %2x %2x %2x %2x %2x %2x\t" + (8 - len) * 4,
                      arr + 0, arr + 1, arr + 2, arr + 3, arr + 4, arr + 5, arr + 6, arr + 7, arr + 8) != (int)len)
            {
                continue;
            }
            memset(arr + len, 0x00, 8 - len);
//            data = *(uint64_t*)arr;
//            memcpy(&data, arr, 8);
            data =
                    ((uint64_t)arr[0] << 0) |
                    ((uint64_t)arr[1] << 8) |
                    ((uint64_t)arr[2] << 16) |
                    ((uint64_t)arr[3] << 24) |
                    ((uint64_t)arr[4] << 32) |
                    ((uint64_t)arr[5] << 40) |
                    ((uint64_t)arr[6] << 48) |
                    ((uint64_t)arr[7] << 56);

            //data =
            //	(((data >> 0) & 0xffULL) << 56) |
            //	(((data >> 8) & 0xffULL) << 48) |
            //	(((data >> 16) & 0xffULL) << 40) |
            //	(((data >> 24) & 0xffULL) << 32) |
            //	(((data >> 32) & 0xffULL) << 24) |
            //	(((data >> 40) & 0xffULL) << 16) |
            //	(((data >> 48) & 0xffULL) << 8) |
            //	(((data >> 56) & 0xffULL) << 0);

            s += us * 1.0e-6;

            double dt = (double)((unsigned int)((s * 1.0e6) + 60.0e6 - (prevs * 1.0e6)) % (unsigned int)(60.0e6)) * 1.0e-6;

//            if(dt > 0.1)
//            {
//                qDebug() << QString().asprintf("DTM1 %d %6g", i, time);
//                if(i > 0) qDebug() << lines[i - 1];
//                qDebug() << lines[i];
//            }

            time += dt;

            prevs = s;

            m.id = id;
            m.data = data;
            m.time = time;

            auto& messages = channels[ch].messages;
            auto& mapMessagesById = channels[ch].mapMessagesById;

            if(!mapMessagesById.count(id))
            {
                messages.push_back(std::deque<Message>());
                mapMessagesById.insert(std::pair(id, &messages.back()));
            }

            mapMessagesById[id]->push_back(m);
        }
        catch(const std::exception& ex)
        {
            continue;
        }
	}

	delete fileData;
}


