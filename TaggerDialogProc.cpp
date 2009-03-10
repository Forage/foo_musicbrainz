#include "foo_musicbrainz.h"

BOOL CALLBACK TaggerDialogProc(HWND tagger_dialog, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
	case WM_INITDIALOG:
		{
			HWND release_list = GetDlgItem(tagger_dialog, IDC_RELEASE_LIST);
			HWND track_list = GetDlgItem(tagger_dialog, IDC_TRACK_LIST);
			HWND type = GetDlgItem(tagger_dialog, IDC_TYPE);
			HWND status = GetDlgItem(tagger_dialog, IDC_STATUS);
			
			LVCOLUMN column_item;
			uconvert str;

			SetProp(tagger_dialog, L"TrackList", new track_list_view_edit(GetDlgItem(tagger_dialog, IDC_TRACK_LIST)));
			
			// List view styles
			ListView_SetExtendedListViewStyleEx(release_list, LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP, LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP);
			ListView_SetExtendedListViewStyleEx(track_list, LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP, LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP);

			// Adding release list columns
			column_item.mask = LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; 
			wchar_t *release_list_columns[] = { L"Artist", L"Release", L"Date" };
			int col_size[] = { 180, 180, 70 };
			for (int i = 0; i < 3; i++) 
			{ 
				column_item.iSubItem = i;
				column_item.pszText = release_list_columns[i];
				column_item.cx = col_size[i];
				ListView_InsertColumn(release_list, i, &column_item);
			}

			// Adding track list columns
			// Fake column
			column_item.mask = 0;
			ListView_InsertColumn(track_list, 0, &column_item);

			column_item.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
			column_item.fmt = LVCFMT_RIGHT;
			column_item.cx = 30;
			column_item.pszText = L"#";
			ListView_InsertColumn(track_list, 1, &column_item);

			column_item.mask = LVCF_WIDTH | LVCF_TEXT;
			column_item.cx = 280;
			column_item.pszText = L"Title";
			ListView_InsertColumn(track_list, 2, &column_item);

			column_item.mask = LVCF_WIDTH | LVCF_TEXT;
			column_item.cx = 110;
			column_item.pszText = L"Track artist";
			ListView_InsertColumn(track_list, 3, &column_item);

			ListView_DeleteColumn(track_list, 0);

			for (int i = 0; i < MB_RELEASE_TYPES; i++)
			{
				SendMessage(type, CB_ADDSTRING, 0, (LPARAM)str.ToUtf16(mbRelease::Types[i]));
			}
			for (int i = 0; i < MB_RELEASE_STATUSES; i++)
			{
				SendMessage(status, CB_ADDSTRING, 0, (LPARAM)str.ToUtf16(mbRelease::Statuses[i]));
			}
			break;
		}

	case WM_FOO_MB_UPDATE_RELEASES_LIST:
		{
			HWND release_list = GetDlgItem(tagger_dialog, IDC_RELEASE_LIST);
			LVITEM list_item;
			uconvert str;
			mbCollection *mbc = (mbCollection *)GetProp(tagger_dialog, L"Collection");

			list_item.mask = LVIF_TEXT;
			list_item.iSubItem = 0;

			ListView_DeleteAllItems(release_list);
			for (list_item.iItem = 0; list_item.iItem < (int)mbc->getReleasesCount(); list_item.iItem++)
			{
				list_item.pszText = str.ToUtf16(mbc->getRelease(list_item.iItem)->getArtist());
				ListView_InsertItem(release_list, &list_item);
				ListView_SetItemText(release_list, list_item.iItem, 1, str.ToUtf16(mbc->getRelease(list_item.iItem)->getTitle()));
				ListView_SetItemText(release_list, list_item.iItem, 2, str.ToUtf16(mbc->getRelease(list_item.iItem)->getDate()));
			}

			mbRelease *release = mbc->getRelease();
			wchar_t track_number_str[10];
			HWND track_list = GetDlgItem(tagger_dialog, IDC_TRACK_LIST);

			list_item.mask = LVIF_TEXT;
			list_item.iSubItem = 0;

			for (list_item.iItem = 0; list_item.iItem < (int)release->getTracksCount(); list_item.iItem++)
			{
				//column 1 - track_number
				wsprintf(track_number_str, L"%u", list_item.iItem+1);
				list_item.pszText = track_number_str;
				ListView_InsertItem(track_list, &list_item);
			}
			SendMessage(tagger_dialog, WM_FOO_MB_UPDATE_RELEASE, 0, 0);
			break;
		}

	case WM_FOO_MB_UPDATE_RELEASE:
		{
			mbCollection *mbc = (mbCollection *)GetProp(tagger_dialog, L"Collection");
			mbRelease *release = mbc->getRelease();
			uconvert str;

			uSetDlgItemText(tagger_dialog, IDC_ARTIST, release->getArtist());
			uSetDlgItemText(tagger_dialog, IDC_ALBUM, release->getTitle());
			uSetDlgItemText(tagger_dialog, IDC_DATE, release->getDate());

			SendMessage(GetDlgItem(tagger_dialog, IDC_TYPE), CB_SETCURSEL, (WPARAM)mbc->getRelease()->getType(), 0);
			SendMessage(GetDlgItem(tagger_dialog, IDC_STATUS), CB_SETCURSEL, (WPARAM)mbc->getRelease()->getStatus(), 0);

			// VA?
			HWND track_list = GetDlgItem(tagger_dialog, IDC_TRACK_LIST);
			if (release->va)
			{
				ListView_SetColumnWidth(track_list, 1, 280);
				ListView_SetColumnWidth(track_list, 2, 110);
			}
			else
			{
				ListView_SetColumnWidth(track_list, 1, 390);
				ListView_SetColumnWidth(track_list, 2, 0);
			}

			// Tracks
			for (int iItem = 0; iItem < (int)release->getTracksCount(); iItem++)
			{
				//column 2 - title
				ListView_SetItemText(track_list, iItem, 1, str.ToUtf16(release->getTrack(iItem)->getTitle()));
				ListView_SetItemText(track_list, iItem, 2, str.ToUtf16(release->getTrack(iItem)->getArtist()));
			}


			// Link
			pfc::string8 url;
			url = "<a href=\"http://musicbrainz.org/release/";
			url += release->getId();
			url += ".html\">MusicBrainz release page</a>";
			uSetDlgItemText(tagger_dialog, IDC_URL, url);
			break;
		}

	case WM_CLOSE:
		DestroyWindow(tagger_dialog);
		break;

	case WM_DESTROY:
		delete (track_list_view_edit *)GetProp(tagger_dialog, L"TrackList");
		delete (mbCollection *)GetProp(tagger_dialog, L"Collection");
		break;

	case WM_NCDESTROY:
		RemoveProp(tagger_dialog, L"TrackList");
		RemoveProp(tagger_dialog, L"Collection");
		break;
		
	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code)
		{
		case LVN_ITEMCHANGED:
			if (((LPNMLISTVIEW)lParam)->hdr.hwndFrom == GetDlgItem(tagger_dialog, IDC_RELEASE_LIST) && ((LPNMLISTVIEW)lParam)->iItem != -1 && ((LPNMLISTVIEW)lParam)->uChanged & LVIS_DROPHILITED && ((LPNMLISTVIEW)lParam)->uNewState & LVIS_SELECTED)
			{
				if (((mbCollection *)GetProp(tagger_dialog, L"Collection"))->getCurrentRelease() != ((LPNMITEMACTIVATE)lParam)->iItem)
				{
					((mbCollection *)GetProp(tagger_dialog, L"Collection"))->setCurrentRelease(((LPNMITEMACTIVATE)lParam)->iItem);
					SendMessage(tagger_dialog, WM_FOO_MB_UPDATE_RELEASE, 0, 0);
				}
			}
			break;

		case NM_CLICK:
			if (((LPNMITEMACTIVATE)lParam)->iItem != -1 && ((LPNMHDR)lParam)->hwndFrom == GetDlgItem(tagger_dialog, IDC_TRACK_LIST) && ((LPNMITEMACTIVATE)lParam)->iSubItem != 0)
			{
				((track_list_view_edit *)GetProp(tagger_dialog, L"TrackList"))->start(((LPNMITEMACTIVATE)lParam)->iItem, ((LPNMITEMACTIVATE)lParam)->iSubItem);
				break;
			}

		case NM_RETURN:
			{
				if ((((LPNMHDR)lParam)->hwndFrom == GetDlgItem(tagger_dialog, IDC_URL)) && (((PNMLINK)lParam)->item.iLink == 0))
				{	
					ShellExecute(NULL, L"open", ((PNMLINK)lParam)->item.szUrl, NULL, NULL, SW_SHOW);
				}
				break;
			}

		default:
			return FALSE;
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
			DestroyWindow(tagger_dialog);
			break;

		case IDOK:
			{
				mbCollection *collection = ((mbCollection *)GetProp(tagger_dialog, L"Collection"));
				ShowWindow(tagger_dialog, SW_HIDE);
				static_api_ptr_t<metadb_io_v2>()->update_info_async(*collection->getData(),new service_impl_t<foo_mb_file_info_filter_impl>(tagger_dialog),core_api::get_main_window(), metadb_io_v2::op_flag_delay_ui, NULL);
				break;
			}

		case IDC_TYPE:
			{
				switch (HIWORD(wParam))
				{
				case CBN_SELENDOK:
					{
						mbCollection *mbc = (mbCollection *)GetProp(tagger_dialog, L"Collection");
						mbc->getRelease()->setType(SendMessage((HWND)lParam, CB_GETCURSEL, 0,0));
						break;
					}

				default:
					return FALSE;
				}
				break;
			}

		case IDC_STATUS:
			{
				switch (HIWORD(wParam))
				{
				case CBN_SELENDOK:
					{
						mbCollection *mbc = (mbCollection *)GetProp(tagger_dialog, L"Collection");
						mbc->getRelease()->setStatus(SendMessage((HWND)lParam, CB_GETCURSEL, 0,0));
						break;
					}

				default:
					return FALSE;
				}
				break;
			}

		default:
			return FALSE;
		}
		break;

	default:
		return FALSE;
	}
	return TRUE;
}
