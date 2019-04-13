#include "cinemaplugins_prefs_parent_hook.h"
#include "tannotation.h"

NodeData *CinemaPluginsObject::Alloc() { return NewObjClear(CinemaPluginsObject); }

#define MYGROUP 2000
#define NEWS_DATE 2001
#define NEWS_POST 2002

void CinemaPluginsObject::GetTheNews(BaseContainer *settings)
{
	extern char *Grab2(const char *url);
	String news;
	char *chstr = Grab2("https://cinemaplugins.com/news/");
	if (chstr)
	{
		news.SetCString(chstr);
		delete(chstr); // check tyhis on mac
		if (news.HAS_CONTENT())
		{
			String popen = String("<li>");
			String pclose = String("</li>");
			Int32 pos, from, start = 0;
			Int32 index = NEWS_POST;
			while (news.FindFirst(popen, &pos, start))
			{
				pos = pos + popen.STRING_LENGTH();
				from = pos;
				if (news.FindFirst(pclose, &pos, pos))
				{
					String extract = news.SubStr(from, pos - from);
					settings->SetString(index, extract);

					index++;
					start = from + pclose.STRING_LENGTH();
				}
				else
					break;
			}
		}
	}

}

Bool CinemaPluginsObject::Init(GeListNode* node)
{
	return TRUE;
}

Bool CinemaPluginsObject::Message(GeListNode *node, Int32 type, void *data)
{
	if (type == MSG_DESCRIPTION_COMMAND){
		DescriptionCommand	*dc = (DescriptionCommand *)data;
		Int32	did = dc->DESC_ID[0].id;
		switch (did)	{
		case ANNOTATIONTAG_URL_GO: GeOpenHTML(String("https://cinemaplugins.com/")); break;
		case 1234:		GeOpenHTML(String("https://cinemaplugins.com/support/tickets/new")); break;
		case 1235:		GeOpenHTML(String("https://cinemaplugins.com/support/solutions")); break;
		case 1236:
		{
			BaseContainer *settings = ((BaseList2D*)node)->GetDataInstance();
			if (!settings) break;
			GetTheNews(settings);
			break;

		}
		}
	}
	return SUPER::Message(node, type, data);
}

void CinemaPluginsObject::AddStaticTextString(Description *description, Int32 id, const  String &str)
{
	BaseContainer thenews = GetCustomDataTypeDefault(DTYPE_STATICTEXT);
	thenews.SetBool(DESC_SCALEH, TRUE);
	thenews.SetBool(DESC_EDITABLE, FALSE);
	thenews.SetString(DESC_NAME, str);
	description->SetParameter(DescLevel(id, DTYPE_STATICTEXT, 0), thenews, DescID(MYGROUP));
}

Bool CinemaPluginsObject::GetDDescription(GeListNode *node, Description *description, DESCFLAGS_DESC &flags){
	
	if (!description->LoadDescription(Tannotation)) // yes it is a hack, but it works.
		return FALSE;

	void* handle = description->BrowseInit();
	const BaseContainer* bc = nullptr;
	BaseContainer* settings = nullptr;
	DescID id, gid, parent;
	String support = String("Support");
	// loop through all elements of the description
	while (description->GetNext(handle, &bc, id, gid))
	{
		if (id[0].id == ANNOTATIONTAG_URL_GO)	{
			parent = gid;
			settings = description->GetParameterI(id, nullptr);
			if (settings)			{
				settings->SetString(DESC_SHORT_NAME, String("Visit CinemaPlugins.com..."));
				settings->SetBool(DESC_SCALEH, TRUE);
				settings->SetInt32(DESC_CUSTOMGUI, CUSTOMGUI_BUTTON);
			}
			settings = description->GetParameterI(gid, nullptr);
			if (settings)			{
				settings->SetString(DESC_NAME, support);
				settings->SetString(DESC_SHORT_NAME, support);
				settings->SetInt32(DESC_COLUMNS, 1);
			}
			continue;
		}
		if (id[0].dtype == DTYPE_GROUP)		{
			settings = description->GetParameterI(id, nullptr);
			if (settings)			{
				settings->SetString(DESC_NAME, support);
				settings->SetString(DESC_SHORT_NAME, support);

				//hidden button
			}
			continue;
		}
		settings = description->GetParameterI(id, nullptr);
		settings->SetBool(DESC_HIDE, TRUE);
		settings->SetBool(DESC_GROUPSCALEV, TRUE);
	}
	description->BrowseFree(handle);

	//add a support button
	BaseContainer button = GetCustomDataTypeDefault(DTYPE_BUTTON);
	button.SetInt32(DESC_CUSTOMGUI, CUSTOMGUI_BUTTON);
	button.SetBool(DESC_SCALEH, TRUE);
	button.SetBool(DESC_ALIGNLEFT, TRUE);

	//add a helpdesk support button
	button.SetString(DESC_NAME, String("Create a Support Ticket..."));
	description->SetParameter(DescLevel(1234, DTYPE_BUTTON, 0), button, parent);
	button.SetString(DESC_NAME, String("Visit our Helpdesk..."));
	description->SetParameter(DescLevel(1235, DTYPE_BUTTON, 0), button, parent);

	// add a group
	BaseContainer group = GetCustomDataTypeDefault(DTYPE_GROUP);
	group.SetString(DESC_NAME, String("News.."));
	group.SetBool(DESC_FITH, TRUE);
	group.SetBool(DESC_LAYOUTGROUP, FALSE);
	group.SetBool(DESC_GROUPSCALEV, TRUE);
	group.SetBool(DESC_SCALEH, TRUE);
	group.SetBool(DESC_GUIOPEN, TRUE);
	group.SetBool(DESC_DEFAULT, TRUE);
	group.SetBool(DESC_EDITABLE, FALSE);

	description->SetParameter(DescLevel(MYGROUP, DTYPE_GROUP, 0), group, parent);
	Int32 index = NEWS_POST;
	BaseContainer *data = ((BaseList2D*)node)->GetDataInstance();
	if (data)	{
		while (data->GetType(index) == DA_STRING)	{
			AddStaticTextString(description, index,String()); // neds no title
			index++;
		}
	}

	if (index == NEWS_POST)	{
		button.SetString(DESC_NAME, String("Check for News..."));
		description->SetParameter(DescLevel(1236, DTYPE_BUTTON, 0), button, MYGROUP);
	}

	flags |= DESCFLAGS_DESC_LOADED;
	return TRUE;
}

Bool Register_CinemaPlugins_Prefs_Parent(void)
{
	if (FindPlugin(CINEMAPLUGINS_PREFS_PARENT_HOOK_ID, PLUGINTYPE_PREFS)) return TRUE; //already registered
	
	
	BasePlugin *plug = PrefsDialogObject::Register(
		CINEMAPLUGINS_PREFS_PARENT_HOOK_ID,
		CinemaPluginsObject::Alloc,
		String("CinemaPlugins.com"),
		String(""),
		0,
		PREFS_PRI_UNITS
	);
	return TRUE;
}

//PREFS_PRI_PLUGINS 300002001