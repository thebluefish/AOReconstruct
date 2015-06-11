#include "PstWriter.h"
#include "LogImpl.h"

int main(int argc, char * argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cout<<"Usage: TestApp.exe <Path.pst>\n";
            return 1;
        }
        TestApp::CLogImpl log;
        std::cout<<"Creating file: "<<argv[1]<<"\n";
        PstWriter::CPstWriter pstWriter(argv[1], &log);
        PstWriter::CPstMessage msg;
        //Set message properties
        msg.szMsgClass = L"IPM.Note";
        msg.normalizedSubject = L"Test subject";
        msg.subjectPrefix = L"Re: ";

        msg.flags = 0x00000001; // MSGFLAG_READ

        msg.importance = 2; // 0 - LOW, 1 - NORMAL, 2 - HIGH
        msg.priority = 1;  // -1 - NONURGENT, 0 - NORMAL, 1 - URGENT
        msg.sensitivity = 1; // 0 - NONE, 1 - PERSONAL, 2 - PRIVATE, 3 - COMPANY_CONFIDENTIAL

        msg.szSenderName = msg.szSentRepresentingName = L"Alex Noname";
        msg.szSenderEMail = msg.szSentRepresentingEMail = L"noname@mailbox.com";

        //Recipients
        PstWriter::RecipientInfo info;
        info.address = L"recipTo@addr.com";
        info.name = L"First To recip";
        info.type = 1; //MAPI_TO

        msg.recipients.push_back(info);

        info.address = L"recipTo2@addr.com";
        info.name = L"Second To recip";
        info.type = 1; //MAPI_TO

        msg.recipients.push_back(info);

        info.address = L"cc-recip@addr.com";
        info.name = L"CC recip";
        info.type = 2; //MAPI_CC

        msg.recipients.push_back(info);

        info.address = L"bcc-recip@addr.com";
        info.name = L"BCC recip";
        info.type = 3; //MAPI_BCC

        msg.recipients.push_back(info);


        //Set time
        SYSTEMTIME systemTime;
        ::GetSystemTime(&systemTime);
        FILETIME currentFiletime;
        ::SystemTimeToFileTime(&systemTime, &currentFiletime);
        msg.ftCreationTime = currentFiletime;
        msg.ftDeletedOnTime = currentFiletime;
        msg.ftLastModificationTime = currentFiletime;
        msg.ftReceivedTime = currentFiletime;
        msg.ftSentTime = currentFiletime;
        //Set body
        msg.textBody = L"This is text body \r\n blah-blah-blah";
        std::cout<<"Start writing message\n";
        pstWriter.WriteMessage(&msg, L"Inbox\\asf");
        std::cout<<"Message was written successfully";
    }
    catch(std::exception &ex)
    {
        std::cout<<"Error: "<<ex.what();
    }
    catch(...)
    {
        std::cout<<"Unknown error";
    }
    return 0;
}