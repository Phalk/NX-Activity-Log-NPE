#include "Application.hpp"
#include "utils/Forwarder.hpp"
#include "utils/Lang.hpp"
#include "ui/screen/Settings.hpp"
#include "utils/Utils.hpp"
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include "SimpleIniParser.hpp"

using namespace simpleIniParser;

namespace Screen {
    Settings::Settings(Main::Application * a, ScreenCreate sc) {
        this->app = a;
        this->createReason = sc;
		
        // Create "static" elements
        Aether::Rectangle * r;
        if (!this->app->config()->tImage() || this->app->config()->gTheme() != ThemeType::Custom) {
            r = new Aether::Rectangle(400, 88, 850, 559);
            r->setColour(this->app->theme()->altBG());
            this->addElement(r);
		}
        r = new Aether::Rectangle(30, 87, 1220, 1);
        r->setColour(this->app->theme()->fg());
        this->addElement(r);
        r = new Aether::Rectangle(30, 647, 1220, 1);
        r->setColour(this->app->theme()->fg());
        this->addElement(r);
        Aether::ControlBar * c = new Aether::ControlBar();
        c->addControl(Aether::Button::A, "common.buttonHint.ok"_lang);
        c->addControl(Aether::Button::B, "common.buttonHint.back"_lang);
        c->setDisabledColour(this->app->theme()->text());
        c->setEnabledColour(this->app->theme()->text());
        this->addElement(c);
		
        // Add button callbacks
        this->onButtonPress(Aether::Button::B, [this](){
            if (this->app->isUserPage()) {
                this->app->exit();
				} else {
                this->app->setScreen(ScreenID::UserSelect);
			}
		});
	}
	
    void Settings::installForwarder() {
        // A message box will be shown regardless of the outcome
        this->msgbox->emptyBody();
		
        // Check if forwarder exists
        Utils::Forwarder::initVars();
        if (Utils::Forwarder::installed()) {
            Utils::Forwarder::uninstall();
			
			} else {
            // Show message if .nro isn't in right location
            if (!Utils::Forwarder::prepared()) {
                // Create text and add to overlay
                int bw, bh;
                this->msgbox->getBodySize(&bw, &bh);
                Aether::Element * body = new Aether::Element(0, 0, bw, bh);
                Aether::TextBlock * tb = new Aether::TextBlock(50, 40, "settings.other.replaceBox.error"_lang, 24, bw - 100);
                tb->setColour(this->app->theme()->text());
                body->addElement(tb);
                tb = new Aether::TextBlock(50, tb->y() + tb->h() + 20, "settings.other.replaceBox.errorMsg1"_lang + "\n/switch/NX-Activity-Log/NX-Activity-Log.nro\n\n" + "settings.other.replaceBox.errorMsg2"_lang + "\n" + "settings.other.replaceBox.errorMsg3"_lang, 20, body->w() - 100);
                tb->setColour(this->app->theme()->mutedText());
                body->addElement(tb);
                this->msgbox->setBodySize(bw, tb->y() + tb->h() + 40);
                this->msgbox->setBody(body);
                this->app->addOverlay(this->msgbox);
                return;
			}
			
            Utils::Forwarder::install();
		}
		
        // Create message box based on result
        if (Utils::Forwarder::installed()) {
            int bw, bh;
            this->msgbox->getBodySize(&bw, &bh);
            Aether::Element * body = new Aether::Element(0, 0, bw, bh);
            Aether::TextBlock * tb = new Aether::TextBlock(50, 40, "settings.other.replaceBox.success"_lang, 24, bw - 100);
            tb->setColour(this->app->theme()->text());
            body->addElement(tb);
            std::string str = std::string(Utils::Forwarder::atmosphere() ? "common.atmosphere"_lang + "  " : "") + std::string(Utils::Forwarder::reinx() ? "common.reinx"_lang + "  " : "") + std::string(Utils::Forwarder::sxos() ? "common.sxos"_lang : "");
            tb = new Aether::TextBlock(50, tb->y() + tb->h() + 20, "settings.other.replaceBox.successMsg1"_lang + "\n" + str + "\n\n" + "settings.other.replaceBox.successMsg2"_lang + "\n" + "settings.other.replaceBox.successMsg3"_lang, 20, bw - 100);
            tb->setColour(this->app->theme()->mutedText());
            body->addElement(tb);
            this->msgbox->setBodySize(bw, tb->y() + tb->h() + 40);
            this->msgbox->setBody(body);
            this->app->addOverlay(this->msgbox);
            this->optionPage->setValue("common.enabled"_lang);
			} else {
            this->optionPage->setValue("common.disabled"_lang);
		}
	}
	
    void Settings::preparePopupList(std::string s) {
        // Remove previous overlay
        if (this->popuplist != nullptr) {
            delete this->popuplist;
		}
		
        this->popuplist = new Aether::PopupList(s);
        this->popuplist->setBackLabel("common.buttonHint.back"_lang);
        this->popuplist->setOKLabel("common.buttonHint.ok"_lang);
        this->popuplist->setBackgroundColour(this->app->theme()->altBG());
        this->popuplist->setHighlightColour(this->app->theme()->accent());
        this->popuplist->setLineColour(this->app->theme()->fg());
        this->popuplist->setListLineColour(this->app->theme()->mutedLine());
        this->popuplist->setTextColour(this->app->theme()->text());
	}
	
	/*     void Settings::setupLangOverlay() {
        this->preparePopupList("settings.appearance.language"_lang);
		
        // Add an entry for each language
        Language lang = this->app->config()->gLang();
        for (int i = 0; i < Language::TotalLanguages; i++) {
		Language l = (Language)i;
		this->popuplist->addEntry(toString(l), [this, lang, l](){
		if (lang != l) {
		this->app->config()->setGLang(l);
		Utils::Lang::setLanguage(l);
		this->app->reinitScreens(ScreenCreate::Language);
		}
		}, lang == l);
		}
		
        this->app->addOverlay(this->popuplist);
	} */
	
    void Settings::setupScreenOverlay() {
        this->preparePopupList("settings.launch.screen"_lang);
		
        // Add an entry for each sort method
        ScreenID s = this->app->config()->lScreen();
        this->popuplist->addEntry("common.screen.recentActivity"_lang, [this](){
            this->app->config()->setLScreen(ScreenID::RecentActivity);
            this->optionScreen->setValue("common.screen.recentActivity"_lang);
		}, s == ScreenID::RecentActivity);
        this->popuplist->addEntry("common.screen.allActivity"_lang, [this](){
            this->app->config()->setLScreen(ScreenID::AllActivity);
            this->optionScreen->setValue("common.screen.allActivity"_lang);
		}, s == ScreenID::AllActivity);
		
        this->app->addOverlay(this->popuplist);
	}
	
    void Settings::setupSortOverlay() {
        this->preparePopupList("settings.launch.sort"_lang);
		
        // Add an entry for each sort method
        SortType sort = this->app->config()->lSort();
        for (int i = 0; i < SortType::TotalSorts; i++) {
            SortType s = (SortType)i;
            std::string str = toString(s);
            this->popuplist->addEntry(str, [this, str, s](){
                this->app->config()->setLSort(s);
                this->optionSort->setValue(str);
			}, sort == s);
		}
		
        this->app->addOverlay(this->popuplist);
	}
	
    void Settings::setupThemeOverlay() {
        this->preparePopupList("settings.appearance.theme.heading"_lang);
		
        // Add an entry for each theme type
        ThemeType theme = this->app->config()->gTheme();
        for (int i = 0; i < ThemeType::TotalThemes; i++) {
            ThemeType t = (ThemeType)i;
            this->popuplist->addEntry(toString(t), [this, theme, t](){
                if (theme != t) {
                    this->app->config()->setGTheme(t);
                    this->app->theme()->setTheme(t);
                    this->app->reinitScreens(ScreenCreate::Theme);
				}
			}, theme == t);
		}
		
        this->app->addOverlay(this->popuplist);
	}
	
    void Settings::setupViewOverlay() {
        this->preparePopupList("settings.launch.view"_lang);
		
        // Add an entry for each sort method
        ViewPeriod view = this->app->config()->lView();
        for (int i = 0; i < ViewPeriod::TotalViews; i++) {
            ViewPeriod v = (ViewPeriod)i;
            std::string str = toString(v);
            this->popuplist->addEntry(str, [this, str, v](){
                this->app->config()->setLView(v);
                this->optionView->setValue(str);
			}, view == v);
		}
		
        this->app->addOverlay(this->popuplist);
	}
	
    void Settings::onLoad() {
        // Create heading using user's name
        this->heading = new Aether::Text(150, 45, Utils::formatHeading(this->app->activeUser()->username()), 28);
        this->heading->setY(this->heading->y() - this->heading->h()/2);
        this->heading->setColour(this->app->theme()->text());
        this->addElement(this->heading);
		
        // Render user's image
        this->image = new Aether::Image(65, 14, this->app->activeUser()->imgPtr(), this->app->activeUser()->imgSize(), Aether::Render::Wait);
        this->image->setScaleDimensions(60, 60);
        this->image->renderSync();
        this->addElement(this->image);
		
        // Create side menu
        this->menu = new Aether::Menu(30, 88, 388, 559);
        this->menu->addElement(new Aether::MenuOption("common.screen.recentActivity"_lang, this->app->theme()->accent(), this->app->theme()->text(), [this](){
            this->app->setScreen(ScreenID::RecentActivity);
		}));
        this->menu->addElement(new Aether::MenuOption("common.screen.allActivity"_lang, this->app->theme()->accent(), this->app->theme()->text(), [this](){
            this->app->setScreen(ScreenID::AllActivity);
		}));
        this->menu->addElement(new Aether::MenuSeparator(this->app->theme()->mutedLine()));
        Aether::MenuOption * opt = new Aether::MenuOption("common.screen.settings"_lang, this->app->theme()->accent(), this->app->theme()->text(), nullptr);
        this->menu->addElement(opt);
        this->menu->setActiveOption(opt);
        this->menu->setFocussed(opt);
        this->addElement(this->menu);
		
        // Create list
        this->list = new Aether::List(420, 88, 810, 559);
        this->list->setCatchup(11);
        this->list->setScrollBarColour(this->app->theme()->mutedLine());
		
        // ===== UPLOAD TO NINTENDO PROFILES =====
        Aether::ListButton * lb = new Aether::ListButton("settings.importExport.upload"_lang, [this]() {
            this->app->pushScreen();
            this->app->setScreen(ScreenID::Update);
		});
		/* lb->setLineColour(this->app->theme()->mutedLine());
			lb->setTextColour(this->app->theme()->text());
		this->list->addElement(lb); */
		//Aether::ListComment * lc = new Aether::ListComment("settings.updateHint"_lang);
		//lc->setTextColour(this->app->theme()->mutedText());
		//this->list->addElement(lc);
		/* this->list->addElement(new Aether::ListSeparator()); */
		
		// EXPORT DO PHALK.NET/PROFILES
		lb = new Aether::ListButton("settings.importExport.upload"_lang, [this]() {
			this->app->exportToJSON();
			this->msgbox->emptyBody();
			int argc;
			char *argv[100];
			
			CURL *curl;
			CURLcode res;
			
			curl_mime *form = NULL;
			curl_mimepart *field = NULL;
			struct curl_slist *headerlist = NULL;
			static const char buf[] = "Expect:";
			
			curl_global_init(CURL_GLOBAL_ALL);
			
			curl = curl_easy_init();
			if(curl) {
				/* Create the form */
				form = curl_mime_init(curl);
				
				/* Fill in the file upload field */
				field = curl_mime_addpart(form);
				curl_mime_name(field, "json_file");
				curl_mime_filedata(field, "/switch/NX-Activity-Log/export.json");
				
				// get ini user and password
				Ini * login_data = Ini::parseFile("/switch/NX-Activity-Log/login.txt");
				IniOption * user_name = login_data->findSection("SwitchProfiles")->findFirstOption("user_name", false);
				
				/* Fill in the filename field */
				field = curl_mime_addpart(form);
				curl_mime_name(field, "user_name");
				curl_mime_data(field, user_name->value.c_str(), CURL_ZERO_TERMINATED);
				
				
				IniOption * password = login_data->findSection("SwitchProfiles")->findFirstOption("password", false);
				
				/* Fill in the filename field */
				field = curl_mime_addpart(form);
				curl_mime_name(field, "password");
				curl_mime_data(field, password->value.c_str(), CURL_ZERO_TERMINATED);
				
				
				/* Fill in the submit field too, even if this is rarely needed */
				field = curl_mime_addpart(form);
				curl_mime_name(field, "submit");
				curl_mime_data(field, "send", CURL_ZERO_TERMINATED);
				
				/* initialize custom header list (stating that Expect: 100-continue is not
				wanted */
				headerlist = curl_slist_append(headerlist, buf);
				/* what URL that receives this POST */
				curl_easy_setopt(curl, CURLOPT_URL, "http://phalk.net/nintendo/json_curl.php");
				if((argc == 2) && (!strcmp(argv[1], "noexpectheader")))
				/* only disable 100-continue header if explicitly requested */
				curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
				curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);
				
				/* Perform the request, res will get the return code */
				res = curl_easy_perform(curl);
				/* Check for errors */
				if(res != CURLE_OK) {
					// Create text and add to overlay
					int bw, bh;
					this->msgbox->getBodySize(&bw, &bh);
					Aether::Element * body = new Aether::Element(0, 0, bw, bh);
					Aether::TextBlock * tb = new Aether::TextBlock(50, 40, "settings.other.uploadNPE.error"_lang, 24, bw - 100);
					tb->setColour(this->app->theme()->text());
					body->addElement(tb);
					tb = new Aether::TextBlock(50, tb->y() + tb->h() + 20, "settings.other.uploadNPE.errorMsg1"_lang, 20, body->w() - 100);
					tb->setColour(this->app->theme()->mutedText());
					body->addElement(tb);
					this->msgbox->setBodySize(bw, tb->y() + tb->h() + 40);
					this->msgbox->setBody(body);
					this->app->addOverlay(this->msgbox);
				}
				else {
					// Create text and add to overlay
					int bw, bh;
					this->msgbox->getBodySize(&bw, &bh);
					Aether::Element * body = new Aether::Element(0, 0, bw, bh);
					Aether::TextBlock * tbs = new Aether::TextBlock(50, 40, "settings.other.uploadNPE.success"_lang, 24, bw - 100);
					tbs->setColour(this->app->theme()->text());
					body->addElement(tbs);
					tbs = new Aether::TextBlock(50, tbs->y() + tbs->h() + 20, "settings.other.uploadNPE.successMsg1"_lang + "\n\n" + "settings.other.uploadNPE.successMsg2"_lang + "\n" + "settings.other.uploadNPE.successMsg3"_lang, 20, body->w() - 100);
					tbs->setColour(this->app->theme()->mutedText());
					body->addElement(tbs);
					this->msgbox->setBodySize(bw, tbs->y() + tbs->h() + 40);
					this->msgbox->setBody(body);
					this->app->addOverlay(this->msgbox);
				}
				
				/* always cleanup */
				curl_easy_cleanup(curl);
				
				/* then cleanup the form */
				curl_mime_free(form);
				/* free slist */
				curl_slist_free_all(headerlist);
				return;
			}
		});
		lb->setLineColour(this->app->theme()->mutedLine());
		lb->setTextColour(this->app->theme()->text());
		this->list->addElement(lb);
		Aether::ListComment * lc = new Aether::ListComment("settings.importExport.uploadHint"_lang);
		lc->setTextColour(this->app->theme()->mutedText());
		this->list->addElement(lc);
		
		Aether::ListHeading * lh = new Aether::ListHeading("settings.appearance.heading"_lang);
        lh->setRectColour(this->app->theme()->mutedLine());
        lh->setTextColour(this->app->theme()->text());
		
        // LANGUAGE
		/*  this->optionLang = new Aether::ListOption("settings.appearance.language"_lang, toString(this->app->config()->gLang()), [this](){
			this->setupLangOverlay();
			});
			this->optionLang->setHintColour(this->app->theme()->text());
			this->optionLang->setValueColour(this->app->theme()->accent());
			this->optionLang->setLineColour(this->app->theme()->mutedLine());
		this->list->addElement(this->optionLang); */
		
        // THEME
        this->optionTheme = new Aether::ListOption("settings.appearance.theme.heading"_lang, toString(this->app->config()->gTheme()), [this](){
			this->setupThemeOverlay();
		});
        this->optionTheme->setHintColour(this->app->theme()->text());
        this->optionTheme->setValueColour(this->app->theme()->accent());
        this->optionTheme->setLineColour(this->app->theme()->mutedLine());
        this->list->addElement(this->optionTheme);
        lc = new Aether::ListComment("settings.appearance.themeHint"_lang);
        lc->setTextColour(this->app->theme()->mutedText());
        this->list->addElement(lc);
		
        this->optionThemeEdit = new Aether::ListButton("settings.appearance.themeEdit"_lang, [this]() {
			this->app->pushScreen();
			this->app->setScreen(ScreenID::CustomTheme);
		});
        this->optionThemeEdit->setLineColour(this->app->theme()->mutedLine());
        // Customize button only enabled if theme is set to custom
        if (this->app->config()->gTheme() == ThemeType::Custom) {
			this->optionThemeEdit->setTextColour(this->app->theme()->text());
			} else {
			this->optionThemeEdit->setTextColour(this->app->theme()->mutedText());
			this->optionThemeEdit->setSelectable(false);
			this->optionThemeEdit->setTouchable(false);
		}
        this->list->addElement(this->optionThemeEdit);
		
        this->list->addElement(new Aether::ListSeparator());
		
        // GRAPH VALUES
        this->optionGraph = new Aether::ListOption("settings.appearance.graph"_lang, (this->app->config()->gGraph() ? "common.yes"_lang : "common.no"_lang), [this](){
			this->app->config()->setGGraph(!this->app->config()->gGraph());
			this->optionGraph->setValue((this->app->config()->gGraph() ? "common.yes"_lang : "common.no"_lang));
			this->optionGraph->setValueColour((this->app->config()->gGraph() ? this->app->theme()->accent() : this->app->theme()->mutedText()));
		});
        this->optionGraph->setHintColour(this->app->theme()->text());
        this->optionGraph->setValueColour((this->app->config()->gGraph() ? this->app->theme()->accent() : this->app->theme()->mutedText()));
        this->optionGraph->setLineColour(this->app->theme()->mutedLine());
        this->list->addElement(this->optionGraph);
		
        // USE 24H
        this->option24H = new Aether::ListOption("settings.appearance.24H"_lang, (this->app->config()->gIs24H() ? "common.yes"_lang : "common.no"_lang), [this](){
			this->app->config()->setGIs24H(!this->app->config()->gIs24H());
			this->option24H->setValue((this->app->config()->gIs24H() ? "common.yes"_lang : "common.no"_lang));
			this->option24H->setValueColour((this->app->config()->gIs24H() ? this->app->theme()->accent() : this->app->theme()->mutedText()));
		});
        this->option24H->setHintColour(this->app->theme()->text());
        this->option24H->setValueColour((this->app->config()->gIs24H() ? this->app->theme()->accent() : this->app->theme()->mutedText()));
        this->option24H->setLineColour(this->app->theme()->mutedLine());
        this->list->addElement(this->option24H);
		
        this->list->addElement(new Aether::ListSeparator());
		
		
		// ===== IMPORT/EXPORT =====
        lh = new Aether::ListHeading("settings.importExport.heading"_lang);
        lh->setRectColour(this->app->theme()->mutedLine());
        lh->setTextColour(this->app->theme()->text());
        this->list->addElement(lh);
		
        this->list->addElement(new Aether::ListSeparator(20));
		
        // EXPORT
        lb = new Aether::ListButton("settings.importExport.export"_lang, [this]() {
			this->app->exportToJSON();
		});
		
		lb->setLineColour(this->app->theme()->mutedLine());
		lb->setTextColour(this->app->theme()->text());
		this->list->addElement(lb);
		lc = new Aether::ListComment("settings.importExport.exportHint"_lang);
		/* 
			// DELETE IMPORTED DATA
			this->optionDeleteImport = new Aether::ListButton("settings.importExport.deleteImport"_lang, [this]() {
			// TODO: show dialog box
			});
			this->optionDeleteImport->setLineColour(this->app->theme()->mutedLine());
			this->optionDeleteImport->setTextColour(this->app->theme()->text());
			// TODO: uncomment
			// if (!this->app->playData->hasImportedData()) {
			//     // Disable if no imported data
			//     this->optionDeleteImport->setSelectable(false);
			//     this->optionDeleteImport->setTouchable(false);
			// }
			this->list->addElement(this->optionDeleteImport);
		lc = new Aether::ListComment("settings.importExport.deleteImportHint"_lang); */
		lc->setTextColour(this->app->theme()->mutedText());
		this->list->addElement(lc);
		
		this->list->addElement(new Aether::ListSeparator(20));
		
        // ===== APPEARANCE =====
		/*  Aether::ListHeading * lh = new Aether::ListHeading("settings.appearance.heading"_lang);
			lh->setRectColour(this->app->theme()->mutedLine());
		lh->setTextColour(this->app->theme()->text()); */
        /* this->list->addElement(lh); */
		
        this->list->addElement(new Aether::ListSeparator(20));
		
        // ===== LAUNCH OPTIONS =====
        lh = new Aether::ListHeading("settings.launch.heading"_lang);
        lh->setRectColour(this->app->theme()->mutedLine());
        lh->setTextColour(this->app->theme()->text());
        this->list->addElement(lh);
		
        this->list->addElement(new Aether::ListSeparator(20));
		
        // SCREEN
        std::string str = "";
        switch (this->app->config()->lScreen()) {
			case ScreenID::RecentActivity:
			str = "common.screen.recentActivity"_lang;
			break;
			
			case ScreenID::AllActivity:
			str = "common.screen.allActivity"_lang;
			break;
			
			default:
			// Never called but I don't like compiler warnings
			break;
		}
        this->optionScreen = new Aether::ListOption("settings.launch.screen"_lang, str, [this]() {
			this->setupScreenOverlay();
		});
        this->optionScreen->setHintColour(this->app->theme()->text());
        this->optionScreen->setValueColour(this->app->theme()->accent());
        this->optionScreen->setLineColour(this->app->theme()->mutedLine());
        this->list->addElement(this->optionScreen);
        lc = new Aether::ListComment("settings.launch.screenHint"_lang);
        lc->setTextColour(this->app->theme()->mutedText());
        this->list->addElement(lc);
		
        // SORT METHOD
        this->optionSort = new Aether::ListOption("settings.launch.sort"_lang, toString(this->app->config()->lSort()), [this](){
			this->setupSortOverlay();
		});
        this->optionSort->setHintColour(this->app->theme()->text());
        this->optionSort->setValueColour(this->app->theme()->accent());
        this->optionSort->setLineColour(this->app->theme()->mutedLine());
        this->list->addElement(this->optionSort);
        lc = new Aether::ListComment("settings.launch.sortHint"_lang);
        lc->setTextColour(this->app->theme()->mutedText());
        this->list->addElement(lc);
		
        // VIEW
        this->optionView = new Aether::ListOption("settings.launch.view"_lang, toString(this->app->config()->lView()), [this](){
			this->setupViewOverlay();
		});
        this->optionView->setHintColour(this->app->theme()->text());
        this->optionView->setValueColour(this->app->theme()->accent());
        this->optionView->setLineColour(this->app->theme()->mutedLine());
        this->list->addElement(this->optionView);
        lc = new Aether::ListComment("settings.launch.viewHint"_lang);
        lc->setTextColour(this->app->theme()->mutedText());
        this->list->addElement(lc);
		
        this->list->addElement(new Aether::ListSeparator());
		
		
		// ===== OTHERS =====
		lh = new Aether::ListHeading("settings.other.heading"_lang);
		lh->setRectColour(this->app->theme()->mutedLine());
		lh->setTextColour(this->app->theme()->text());
		this->list->addElement(lh);
		
		this->list->addElement(new Aether::ListSeparator(20));
		
		// HIDE DELETED
		str = (this->app->config()->hDeleted() ? "common.yes"_lang : "common.no"_lang);
		this->optionDeleted = new Aether::ListOption("settings.other.hideDeleted"_lang, str, [this](){
			this->app->config()->setHDeleted(!this->app->config()->hDeleted());
			this->optionDeleted->setValue((this->app->config()->hDeleted() ? "common.yes"_lang : "common.no"_lang));
			this->optionDeleted->setValueColour((this->app->config()->hDeleted() ? this->app->theme()->accent() : this->app->theme()->mutedText()));
		});
		this->optionDeleted->setHintColour(this->app->theme()->text());
		this->optionDeleted->setValueColour((this->app->config()->hDeleted() ? this->app->theme()->accent() : this->app->theme()->mutedText()));
		this->optionDeleted->setLineColour(this->app->theme()->mutedLine());
		this->list->addElement(this->optionDeleted);
		lc = new Aether::ListComment("settings.other.hideDeletedHint"_lang);
		lc->setTextColour(this->app->theme()->mutedText());
		this->list->addElement(lc);
		
		// REPLACE USER PAGE
		Utils::Forwarder::initVars();
		this->optionPage = new Aether::ListOption("settings.other.replace"_lang, (Utils::Forwarder::installed() ? "common.enabled"_lang : "common.disabled"_lang), [this](){
			this->installForwarder();
		});
		this->optionPage->setHintColour(this->app->theme()->text());
		this->optionPage->setValueColour(this->app->theme()->accent());
		this->optionPage->setLineColour(this->app->theme()->mutedLine());
		this->list->addElement(this->optionPage);
		
		lc = new Aether::ListComment("settings.other.replaceHint"_lang);
		lc->setTextColour(this->app->theme()->mutedText());
		this->list->addElement(lc);
		
		this->list->addElement(new Aether::ListSeparator());
		
		// INFORMATION
		lc = new Aether::ListComment("http://www.phalk.net/nintendo/");
		lc->setTextColour(this->app->theme()->mutedText());
		this->list->addElement(lc);
		lc = new Aether::ListComment("NX Activity Log v" + std::string(VER_STRING) + "\n" + "settings.about"_lang + "\n\n" + "settings.support"_lang + "\nhttps://ko-fi.com/tallbl0nde");
		lc->setTextColour(this->app->theme()->mutedText());
		this->list->addElement(lc);
		
		this->addElement(this->list);
		
		// Create base message box
		this->msgbox = new Aether::MessageBox();
		this->msgbox->addTopButton("common.buttonHint.ok"_lang, [this](){
			this->msgbox->close();
		});
		this->msgbox->setLineColour(this->app->theme()->mutedLine());
		this->msgbox->setRectangleColour(this->app->theme()->altBG());
		this->msgbox->setTextColour(this->app->theme()->accent());
		
		// Show update icon if needbe
		/* this->updateElm =nullptr;
			if (this->app->hasUpdate()) {
			this->updateElm = new Aether::Image(50, 669, "romfs:/icon/download.png");
			this->updateElm->setColour(this->app->theme()->text());
			this->addElement(this->updateElm);
		} */
		
		this->popuplist = nullptr;
		
		// Activate appropriate item if necessary
		if (this->createReason == ScreenCreate::Language) {
			this->setFocussed(this->list);
			//this->list->setFocussed(this->optionLang);
			} else if (this->createReason == ScreenCreate::Theme) {
			this->setFocussed(this->list);
			this->list->setFocussed(this->optionTheme);
			} else if (this->createReason == ScreenCreate::ThemeEdit) {
			this->setFocussed(this->list);
			this->list->setFocussed(this->optionThemeEdit);
		}
		
		this->createReason = ScreenCreate::Normal;
	}
	
	void Settings::onUnload() {
		this->removeElement(this->heading);
		this->removeElement(this->image);
		this->removeElement(this->list);
		this->removeElement(this->menu);
		this->removeElement(this->updateElm);
		delete this->msgbox;
		delete this->popuplist;
	}
};
