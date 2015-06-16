#pragma once

#include <mapidefs.h>
#include <mapix.h>
#include "PstTempl.h"

struct IPF_t 
{
	SBinary Appointment; // Calendar
	SBinary Contact;     // Contacts
	SBinary Journal;     // Journal
	SBinary Note;        // Drafts
	SBinary StickyNote;  // Notes
	SBinary Task;        // Tasks
};

typedef COM::AutoPtr<IMAPISession,ComEraser> MapiSession;
typedef COM::AutoPtr<IProfAdmin,ComEraser > ProfAdmin;
typedef COM::AutoPtr<IMsgServiceAdmin,ComEraser > MsgServiceAdmin;
typedef COM::AutoPtr<IMAPITable,ComEraser > MapiTable;
typedef COM::AutoPtr<IMsgStore,ComEraser > MsgStore;
typedef COM::AutoPtr<IMAPIFolder,ComEraser > MapiFolder;
typedef COM::AutoPtr<SPropValue,MapiEraser > MapiProp;
typedef COM::AutoPtr<ENTRYID,MapiEraser> MapiEntry;
typedef COM::AutoPtr<IStream,ComEraser> MapiStream;
typedef __int64 ID;

const unsigned int SUBJECT_TAG = 1;
const unsigned int TEXT_BODY_TAG = 1;
