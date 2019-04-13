#ifndef __DEM_EARTH_PREFS_PARENT_HOOK_H
#define __DEM_EARTH_PREFS_PARENT_HOOK_H

#include "lib_prefs.h"
#include "c4d_customdatatypeplugin.h"
#include "c4d_general.h"

#define CINEMAPLUGINS_PREFS_PARENT_HOOK_ID		1050198

#if API_VERSION > 20000
#define DESCFLAGS_DESC_LOADED	DESCFLAGS_DESC::LOADED
#define PLUGINTYPE_PREFS		PLUGINTYPE::PREFS
#define DESC_ID _descId
#define HAS_CONTENT IsPopulated
#define STRING_LENGTH GetCStringLen
#endif
#if API_VERSION < 20000
#define DESC_ID id
#define HAS_CONTENT Content
#define STRING_LENGTH GetLength

#endif



class CinemaPluginsObject : public PrefsDialogObject
{
	INSTANCEOF(CinemaPluginsObject, PrefsDialogObject)
public:
	static NodeData *Alloc();
	Bool GetDDescription(GeListNode *node, Description *description, DESCFLAGS_DESC &flags);
	virtual Bool Message(GeListNode *node, Int32 type, void *data);
	virtual Bool Init(GeListNode* node);


private:
	void GetTheNews(BaseContainer *settings);
	void AddStaticTextString(Description *description, Int32 id, const  String &str);

};

Bool Register_CinemaPlugins_Prefs_Parent(void);

#endif