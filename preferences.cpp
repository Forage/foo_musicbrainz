#include "foo_musicbrainz.h"

static const GUID guid_cfg_short_date = { 0x18734618, 0x7920, 0x4d24, { 0x84, 0xa1, 0xb9, 0xd6, 0x6e, 0xd8, 0x25, 0xbc } };
cfg_bool cfg_short_date(guid_cfg_short_date, false);

static const GUID guid_cfg_no_feat = { 0x98f9ce46, 0xe0c5, 0x48f4, { 0xa6, 0x3e, 0x19, 0x34, 0x39, 0x64, 0x35, 0x55 } };
cfg_bool cfg_no_feat(guid_cfg_no_feat, false);

static const GUID guid_cfg_write_ids = { 0x8c8b61e0, 0x8eea, 0x4c34, { 0x9a, 0x51, 0x4d, 0xa9, 0x9c, 0xec, 0xcb, 0xbc } };
cfg_bool cfg_write_ids(guid_cfg_write_ids, true);

static const GUID guid_cfg_albumtype = { 0xd8fdb1d8, 0xde2, 0x4f1f, { 0x85, 0x5a, 0xc0, 0x5, 0x98, 0x60, 0x9b, 0xe9 } };
cfg_bool cfg_albumtype(guid_cfg_albumtype, true);

static const GUID guid_cfg_albumstatus = { 0x57a8dddd, 0xdf24, 0x4fd3, { 0xac, 0x95, 0x6f, 0x8, 0x3, 0x26, 0x41, 0x7c } };
cfg_bool cfg_albumstatus(guid_cfg_albumstatus, true);

static const GUID guid_cfg_albumtype_data = { 0x20968c09, 0xb0d4, 0x4059, { 0xb8, 0x92, 0xda, 0x76, 0xf8, 0xe6, 0x51, 0x4e } };
cfg_string cfg_albumtype_data(guid_cfg_albumtype_data, "MUSICBRAINZ_ALBUMTYPE");

static const GUID guid_cfg_albumstatus_data = { 0x77182aac, 0x1caa, 0x4793, { 0xb7, 0x15, 0xcc, 0xf8, 0x97, 0xba, 0x11, 0x1a } };
cfg_string cfg_albumstatus_data(guid_cfg_albumstatus_data, "MUSICBRAINZ_ALBUMSTATUS");

class PreferencesPageInstance : public CDialogImpl<PreferencesPageInstance>, public preferences_page_instance {
private:
	CButton short_date_checkbox;
	CButton no_feat_checkbox;
	CButton write_ids_checkbox;
	CButton write_albumtype_checkbox;
	CButton write_albumstatus_checkbox;
	CEdit albumtype;
	CEdit albumstatus;
	preferences_page_callback::ptr on_change_callback;

public:
	PreferencesPageInstance(preferences_page_callback::ptr callback) : on_change_callback(callback) {}

	enum { IDD = IDD_PREFERENCES };

	BEGIN_MSG_MAP(CPreferencesDialog)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_HANDLER_EX(IDC_SHORT_DATE, BN_CLICKED, OnChanged)
		COMMAND_HANDLER_EX(IDC_NO_FEAT, BN_CLICKED, OnChanged)
		COMMAND_HANDLER_EX(IDC_WRITE_IDS, BN_CLICKED, OnChanged)
		COMMAND_HANDLER_EX(IDC_ALBUMTYPE, BN_CLICKED, OnAlbumType)
		COMMAND_HANDLER_EX(IDC_ALBUMSTATUS, BN_CLICKED, OnAlbumStatus)
		COMMAND_HANDLER_EX(IDC_ALBUMTYPE_DATA, EN_UPDATE, OnChanged)
		COMMAND_HANDLER_EX(IDC_ALBUMSTATUS_DATA, EN_UPDATE, OnChanged)
	END_MSG_MAP()

	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam) {
		short_date_checkbox = GetDlgItem(IDC_SHORT_DATE);
		no_feat_checkbox = GetDlgItem(IDC_NO_FEAT);
		write_ids_checkbox = GetDlgItem(IDC_WRITE_IDS);
		write_albumtype_checkbox = GetDlgItem(IDC_ALBUMTYPE);
		write_albumstatus_checkbox = GetDlgItem(IDC_ALBUMSTATUS);
		albumtype = GetDlgItem(IDC_ALBUMTYPE_DATA);
		albumstatus = GetDlgItem(IDC_ALBUMSTATUS_DATA);

		short_date_checkbox.SetCheck(cfg_short_date.get_value());
		no_feat_checkbox.SetCheck(cfg_no_feat.get_value());
		write_ids_checkbox.SetCheck(cfg_write_ids.get_value());
		write_albumtype_checkbox.SetCheck(cfg_albumtype.get_value());
		write_albumstatus_checkbox.SetCheck(cfg_albumstatus.get_value());
		uSetWindowText(albumtype, cfg_albumtype_data);
		uSetWindowText(albumstatus, cfg_albumstatus_data);
		if (cfg_albumtype.get_value()) albumtype.EnableWindow(true);
		if (cfg_albumstatus.get_value()) albumstatus.EnableWindow(true);

		return 0;
	}

	bool has_changed() {
		if ((bool)short_date_checkbox.GetCheck() != cfg_short_date.get_value()) return true;
		if ((bool)no_feat_checkbox.GetCheck() != cfg_no_feat.get_value()) return true;
		if ((bool)write_ids_checkbox.GetCheck() != cfg_write_ids.get_value()) return true;
		if ((bool)write_albumtype_checkbox.GetCheck() != cfg_albumtype.get_value()) return true;
		if ((bool)write_albumstatus_checkbox.GetCheck() != cfg_albumstatus.get_value()) return true;
		
		pfc::string8 temp;
		uGetWindowText(albumtype, temp);
		if (cfg_albumtype_data != temp) return true;
		uGetWindowText(albumstatus, temp);
		if (cfg_albumstatus_data != temp) return true;

		return false;
	}

	t_uint32 get_state() {
		t_uint32 state = preferences_state::resettable;
		if (has_changed()) state |= preferences_state::changed;
		return state;
	}

	void apply() {
		cfg_short_date = (bool)short_date_checkbox.GetCheck();
		cfg_no_feat = (bool)no_feat_checkbox.GetCheck();
		cfg_write_ids = (bool)write_ids_checkbox.GetCheck();
		cfg_albumtype = (bool)write_albumtype_checkbox.GetCheck();
		cfg_albumstatus = (bool)write_albumstatus_checkbox.GetCheck();
		uGetWindowText(albumtype, cfg_albumtype_data);
		uGetWindowText(albumstatus, cfg_albumstatus_data);
	}

	void on_change() {
		on_change_callback->on_state_changed();
	}

	void reset() {
		short_date_checkbox.SetCheck(false);
		no_feat_checkbox.SetCheck(false);
		write_ids_checkbox.SetCheck(true);
		write_albumtype_checkbox.SetCheck(true);
		write_albumstatus_checkbox.SetCheck(true);
		uSetWindowText(albumtype, "MUSICBRAINZ_ALBUMTYPE");
		uSetWindowText(albumstatus, "MUSICBRAINZ_ALBUMSTATUS");
		on_change();
	}

	void OnChanged(UINT, int, HWND) {
		on_change();
	}

	void OnAlbumType(UINT, int, CButton) {
		albumtype.EnableWindow((bool)write_albumtype_checkbox.GetCheck());
		on_change();
	}

	void OnAlbumStatus(UINT, int, CButton) {
		albumstatus.EnableWindow((bool)write_albumstatus_checkbox.GetCheck());
		on_change();
	}
};

class PreferencesPage : public preferences_page_impl<PreferencesPageInstance> {
public:
	const char * get_name() {
		return "MusicBrainz Tagger";
	}

	GUID get_guid() {
		static const GUID guid = { 0x79179a37, 0x5942, 0x4fdf, { 0xbb, 0xb7, 0x93, 0xfd, 0x35, 0xfc, 0xfe, 0x97 } };
		return guid;
	}

	GUID get_parent_guid() {
		return preferences_page::guid_tagging;
	}
};

preferences_page_factory_t<PreferencesPage> _;