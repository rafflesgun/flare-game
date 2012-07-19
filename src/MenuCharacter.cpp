/*
Copyright © 2011-2012 Clint Bellanger

This file is part of FLARE.

FLARE is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

FLARE is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
FLARE.  If not, see http://www.gnu.org/licenses/
*/

/**
 * class MenuCharacter
 */

#include "FileParser.h"
#include "Menu.h"
#include "MenuCharacter.h"
#include "SharedResources.h"
#include "Settings.h"
#include "StatBlock.h"
#include "UtilsParsing.h"
#include "WidgetButton.h"
#include "WidgetListBox.h"


using namespace std;


MenuCharacter::MenuCharacter(StatBlock *_stats) {
	stats = _stats;

	skill_points = 0;

	visible = false;
	newPowerNotification = false;

	// Load config settings
	FileParser infile;
	if(infile.open(mods->locate("menus/character.txt"))) {
		while(infile.next()) {
			infile.val = infile.val + ',';

			if(infile.key == "close") {
				close_pos.x = eatFirstInt(infile.val,',');
				close_pos.y = eatFirstInt(infile.val,',');
			} else if(infile.key == "upgrade_physical") {
				upgrade_pos[0].x = eatFirstInt(infile.val,',');
				upgrade_pos[0].y = eatFirstInt(infile.val,',');
			} else if(infile.key == "upgrade_mental") {
				upgrade_pos[1].x = eatFirstInt(infile.val,',');
				upgrade_pos[1].y = eatFirstInt(infile.val,',');
			} else if(infile.key == "upgrade_offense") {
				upgrade_pos[2].x = eatFirstInt(infile.val,',');
				upgrade_pos[2].y = eatFirstInt(infile.val,',');
			} else if(infile.key == "upgrade_defense") {
				upgrade_pos[3].x = eatFirstInt(infile.val,',');
				upgrade_pos[3].y = eatFirstInt(infile.val,',');
			} else if(infile.key == "statlist") {
				statlist_pos.x = eatFirstInt(infile.val,',');
				statlist_pos.y = eatFirstInt(infile.val,',');
			} else if(infile.key == "label_name") {
				label_pos[0].x = eatFirstInt(infile.val,',');
				label_pos[0].y = eatFirstInt(infile.val,',');
			} else if(infile.key == "label_level") {
				label_pos[1].x = eatFirstInt(infile.val,',');
				label_pos[1].y = eatFirstInt(infile.val,',');
			} else if(infile.key == "label_physical") {
				label_pos[2].x = eatFirstInt(infile.val,',');
				label_pos[2].y = eatFirstInt(infile.val,',');
			} else if(infile.key == "label_mental") {
				label_pos[3].x = eatFirstInt(infile.val,',');
				label_pos[3].y = eatFirstInt(infile.val,',');
			} else if(infile.key == "label_offense") {
				label_pos[4].x = eatFirstInt(infile.val,',');
				label_pos[4].y = eatFirstInt(infile.val,',');
			} else if(infile.key == "label_defense") {
				label_pos[5].x = eatFirstInt(infile.val,',');
				label_pos[5].y = eatFirstInt(infile.val,',');
			} else if(infile.key == "name") {
				value_pos[0].x = eatFirstInt(infile.val,',');
				value_pos[0].y = eatFirstInt(infile.val,',');
				value_pos[0].w = eatFirstInt(infile.val,',');
				value_pos[0].h = eatFirstInt(infile.val,',');
			} else if(infile.key == "level") {
				value_pos[1].x = eatFirstInt(infile.val,',');
				value_pos[1].y = eatFirstInt(infile.val,',');
				value_pos[1].w = eatFirstInt(infile.val,',');
				value_pos[1].h = eatFirstInt(infile.val,',');
			} else if(infile.key == "physical") {
				value_pos[2].x = eatFirstInt(infile.val,',');
				value_pos[2].y = eatFirstInt(infile.val,',');
				value_pos[2].w = eatFirstInt(infile.val,',');
				value_pos[2].h = eatFirstInt(infile.val,',');
			} else if(infile.key == "mental") {
				value_pos[3].x = eatFirstInt(infile.val,',');
				value_pos[3].y = eatFirstInt(infile.val,',');
				value_pos[3].w = eatFirstInt(infile.val,',');
				value_pos[3].h = eatFirstInt(infile.val,',');
			} else if(infile.key == "offense") {
				value_pos[4].x = eatFirstInt(infile.val,',');
				value_pos[4].y = eatFirstInt(infile.val,',');
				value_pos[4].w = eatFirstInt(infile.val,',');
				value_pos[4].h = eatFirstInt(infile.val,',');
			} else if(infile.key == "defense") {
				value_pos[5].x = eatFirstInt(infile.val,',');
				value_pos[5].y = eatFirstInt(infile.val,',');
				value_pos[5].w = eatFirstInt(infile.val,',');
				value_pos[5].h = eatFirstInt(infile.val,',');
			} else if(infile.key == "unspent") {
				value_pos[6].x = eatFirstInt(infile.val,',');
				value_pos[6].y = eatFirstInt(infile.val,',');
				value_pos[6].w = eatFirstInt(infile.val,',');
				value_pos[6].h = eatFirstInt(infile.val,',');
			}
		}
	}

	loadGraphics();

	closeButton = new WidgetButton(mods->locate("images/menus/buttons/button_x.png"));

	// Upgrade buttons
	for (int i=0; i<4; i++) {
		upgradeButton[i] = new WidgetButton(mods->locate("images/menus/buttons/upgrade.png"));
		upgradeButton[i]->enabled = false;
	}
	physical_up = false;
	mental_up = false;
	offense_up = false;
	defense_up = false;

	// menu title
	labelCharacter = new WidgetLabel();

	for (int i=0; i<CSTAT_COUNT; i++) {
		cstat[i].label = new WidgetLabel();
		cstat[i].value = new WidgetLabel();
		cstat[i].hover.x = cstat[i].hover.y = 0;
		cstat[i].hover.w = cstat[i].hover.h = 0;
	}

	// stat list
	statList = new WidgetListBox(14, 10, mods->locate("images/menus/buttons/listbox_char.png"));
}

void MenuCharacter::update() {
	// TODO put item position info in a config file

	// close button
	closeButton->pos.x = window_area.x + close_pos.x;
	closeButton->pos.y = window_area.y + close_pos.y;

	// menu title
	labelCharacter->set(window_area.x+window_area.w/2, window_area.y+16, JUSTIFY_CENTER, VALIGN_CENTER, msg->get("Character"), FONT_WHITE);

	// upgrade buttons
	for (int i=0; i<4; i++) {
		upgradeButton[i]->pos.x = window_area.x+upgrade_pos[i].x;
		upgradeButton[i]->pos.y = window_area.y+upgrade_pos[i].y;
	}

	// stat list
	statList->pos.x = window_area.x+statlist_pos.x;
	statList->pos.y = window_area.y+statlist_pos.y;

	// setup static labels
	cstat[CSTAT_NAME].label->set(window_area.x+label_pos[0].x, window_area.y+label_pos[0].y, JUSTIFY_RIGHT, VALIGN_CENTER, msg->get("Name"), FONT_WHITE);
	cstat[CSTAT_LEVEL].label->set(window_area.x+label_pos[1].x, window_area.y+label_pos[1].y, JUSTIFY_RIGHT, VALIGN_CENTER, msg->get("Level"), FONT_WHITE);
	cstat[CSTAT_PHYSICAL].label->set(window_area.x+label_pos[2].x, window_area.y+label_pos[2].y, JUSTIFY_LEFT, VALIGN_CENTER, msg->get("Physical"), FONT_WHITE);
	cstat[CSTAT_MENTAL].label->set(window_area.x+label_pos[3].x, window_area.y+label_pos[3].y, JUSTIFY_LEFT, VALIGN_CENTER, msg->get("Mental"), FONT_WHITE);
	cstat[CSTAT_OFFENSE].label->set(window_area.x+label_pos[4].x, window_area.y+label_pos[4].y, JUSTIFY_LEFT, VALIGN_CENTER, msg->get("Offense"), FONT_WHITE);
	cstat[CSTAT_DEFENSE].label->set(window_area.x+label_pos[5].x, window_area.y+label_pos[5].y, JUSTIFY_LEFT, VALIGN_CENTER, msg->get("Defense"), FONT_WHITE);

	// setup hotspot locations
	cstat[CSTAT_NAME].setHover(window_area.x+value_pos[0].x, window_area.y+value_pos[0].y, value_pos[0].w, value_pos[0].h);
	cstat[CSTAT_LEVEL].setHover(window_area.x+value_pos[1].x, window_area.y+value_pos[1].y, value_pos[1].w, value_pos[1].h);
	cstat[CSTAT_PHYSICAL].setHover(window_area.x+value_pos[2].x, window_area.y+value_pos[2].y, value_pos[2].w, value_pos[2].h);
	cstat[CSTAT_MENTAL].setHover(window_area.x+value_pos[3].x, window_area.y+value_pos[3].y, value_pos[3].w, value_pos[3].h);
	cstat[CSTAT_OFFENSE].setHover(window_area.x+value_pos[4].x, window_area.y+value_pos[4].y, value_pos[4].w, value_pos[4].h);
	cstat[CSTAT_DEFENSE].setHover(window_area.x+value_pos[5].x, window_area.y+value_pos[5].y, value_pos[5].w, value_pos[5].h);
	cstat[CSTAT_UNSPENT].setHover(window_area.x+value_pos[6].x, window_area.y+value_pos[6].y, value_pos[6].w, value_pos[6].h);

}

void MenuCharacter::loadGraphics() {

	background = IMG_Load(mods->locate("images/menus/character.png").c_str());
	if(!background) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
		SDL_Quit();
	}

	// optimize
	SDL_Surface *cleanup = background;
	background = SDL_DisplayFormatAlpha(background);
	SDL_FreeSurface(cleanup);

}

/**
 * Rebuild all stat values and tooltip info
 */
void MenuCharacter::refreshStats() {

	stringstream ss;

	// update stat text
	cstat[CSTAT_NAME].value->set(window_area.x+value_pos[0].x+4, window_area.y+value_pos[0].y+value_pos[0].h/2, JUSTIFY_LEFT, VALIGN_CENTER, stats->name, FONT_WHITE);

	ss.str("");
	ss << stats->level;
	cstat[CSTAT_LEVEL].value->set(window_area.x+value_pos[1].x+value_pos[1].w/2, window_area.y+value_pos[1].y+value_pos[1].h/2, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), FONT_WHITE);

	ss.str("");
	ss << stats->get_physical();
	cstat[CSTAT_PHYSICAL].value->set(window_area.x+value_pos[2].x+value_pos[2].w/2, window_area.y+value_pos[2].y+value_pos[2].h/2, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), bonusColor(stats->physical_additional));

	ss.str("");
	ss << stats->get_mental();
	cstat[CSTAT_MENTAL].value->set(window_area.x+value_pos[3].x+value_pos[3].w/2, window_area.y+value_pos[3].y+value_pos[3].h/2, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), bonusColor(stats->mental_additional));

	ss.str("");
	ss << stats->get_offense();
	cstat[CSTAT_OFFENSE].value->set(window_area.x+value_pos[4].x+value_pos[4].w/2, window_area.y+value_pos[4].y+value_pos[4].h/2, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), bonusColor(stats->offense_additional));

	ss.str("");
	ss << stats->get_defense();
	cstat[CSTAT_DEFENSE].value->set(window_area.x+value_pos[5].x+value_pos[5].w/2, window_area.y+value_pos[5].y+value_pos[5].h/2, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), bonusColor(stats->defense_additional));

	ss.str("");
	if (skill_points > 0) ss << skill_points << " " << msg->get("points remaining");
	else ss.str("");
	cstat[CSTAT_UNSPENT].value->set(window_area.x+value_pos[6].x+value_pos[6].w/2, window_area.y+value_pos[6].y+value_pos[6].h/2, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), FONT_GREEN);
	ss.str("");

	// scrolling stat list
	statList->clear();
	statList->refresh();

	ss.str("");
	ss << "Max HP: " << stats->maxhp;
	statList->append(ss.str(),"Each point of Physical grants +8 HP. Each level grants +2 HP");

	ss.str("");
	ss << "HP Regen: " << stats->hp_per_minute;
	statList->append(ss.str(),"Ticks of HP regen per minute. Each point of Physical grants +4 HP regen. Each level grants +1 HP regen");

	ss.str("");
	ss << "Max MP: " << stats->maxmp;
	statList->append(ss.str(),"Each point of Mental grants +8 MP. Each level grants +2 MP");

	ss.str("");
	ss << "MP Regen: " << stats->hp_per_minute;
	statList->append(ss.str(),"Ticks of MP regen per minute. Each point of Mental grants +4 MP regen. Each level grants +1 MP regen");

	ss.str("");
	ss << "Accuracy (vs level 1): " << stats->accuracy << "%";
	statList->append(ss.str(),"Each point of Offense grants +5 accuracy. Each level grants +1 accuracy");

	ss.str("");
	ss << "Accuracy (vs level 5): " << (stats->accuracy-20) << "%";
	statList->append(ss.str(),"Each point of Offense grants +5 accuracy. Each level grants +1 accuracy");

	ss.str("");
	ss << "Avoidance (vs level 1): " << stats->avoidance << "%";
	statList->append(ss.str(),"Each point of Defense grants +5 avoidance. Each level grants +1 accuracy");

	ss.str("");
	ss << "Avoidance (vs level 5): " << (stats->avoidance-20) << "%";
	statList->append(ss.str(),"Each point of Defense grants +5 avoidance. Each level grants +1 accuracy");

	ss.str("");
	ss << "Melee Damage: ";
	if (stats->dmg_melee_max >= stats->dmg_ment_max)
		ss << stats->dmg_melee_min << "-" << stats->dmg_melee_max;
	else
		ss << stats->dmg_ment_min << "-" << stats->dmg_ment_max;
	statList->append(ss.str(),"");

	ss.str("");
	ss << "Ranged Damage: ";
	if (stats->dmg_ranged_max > 0)
		ss << stats->dmg_ranged_min << "-" << stats->dmg_ranged_max;
	else
		ss << "-";
	statList->append(ss.str(),"");

	ss.str("");
	ss << "Crit: " << stats->crit << "%";
	statList->append(ss.str(),"");

	ss.str("");
	ss << "Absorb: ";
	if (stats->absorb_min == stats->absorb_max)
		ss << stats->absorb_min;
	else
		ss << stats->absorb_min << "-" << stats->absorb_max;
	statList->append(ss.str(),"");

	ss.str("");
	ss << "Fire Resistance: " << (100 - stats->attunement_fire) << "%";
	statList->append(ss.str(),"");

	ss.str("");
	ss << "Ice Resistance: " << (100 - stats->attunement_ice) << "%";
	statList->append(ss.str(),"");

	// update tool tips
	cstat[CSTAT_NAME].tip.num_lines = 0;
	cstat[CSTAT_NAME].tip.lines[cstat[CSTAT_NAME].tip.num_lines++] = msg->get(stats->character_class);

	cstat[CSTAT_LEVEL].tip.num_lines = 0;
	cstat[CSTAT_LEVEL].tip.lines[cstat[CSTAT_LEVEL].tip.num_lines++] = msg->get("XP: %d", stats->xp);
	if (stats->level < MAX_CHARACTER_LEVEL) {
		cstat[CSTAT_LEVEL].tip.lines[cstat[CSTAT_LEVEL].tip.num_lines++] = msg->get("Next: %d", stats->xp_table[stats->level]);
	}

	cstat[CSTAT_PHYSICAL].tip.num_lines = 0;
	cstat[CSTAT_PHYSICAL].tip.lines[cstat[CSTAT_PHYSICAL].tip.num_lines++] = msg->get("Physical (P) increases melee weapon proficiency and total HP.");
	cstat[CSTAT_PHYSICAL].tip.lines[cstat[CSTAT_PHYSICAL].tip.num_lines++] = msg->get("base (%d), bonus (%d)", stats->physical_character, stats->physical_additional);

	cstat[CSTAT_MENTAL].tip.num_lines = 0;
	cstat[CSTAT_MENTAL].tip.lines[cstat[CSTAT_MENTAL].tip.num_lines++] = msg->get("Mental (M) increases mental weapon proficiency and total MP.");
	cstat[CSTAT_MENTAL].tip.lines[cstat[CSTAT_MENTAL].tip.num_lines++] = msg->get("base (%d), bonus (%d)", stats->mental_character, stats->mental_additional);

	cstat[CSTAT_OFFENSE].tip.num_lines = 0;
	cstat[CSTAT_OFFENSE].tip.lines[cstat[CSTAT_OFFENSE].tip.num_lines++] = msg->get("Offense (O) increases ranged weapon proficiency and accuracy.");
	cstat[CSTAT_OFFENSE].tip.lines[cstat[CSTAT_OFFENSE].tip.num_lines++] = msg->get("base (%d), bonus (%d)", stats->offense_character, stats->offense_additional);

	cstat[CSTAT_DEFENSE].tip.num_lines = 0;
	cstat[CSTAT_DEFENSE].tip.lines[cstat[CSTAT_DEFENSE].tip.num_lines++] = msg->get("Defense (D) increases armor proficiency and avoidance.");
	cstat[CSTAT_DEFENSE].tip.lines[cstat[CSTAT_DEFENSE].tip.num_lines++] = msg->get("base (%d), bonus (%d)", stats->defense_character, stats->defense_additional);

	cstat[CSTAT_UNSPENT].tip.num_lines = 0;
	if (skill_points) cstat[CSTAT_UNSPENT].tip.lines[cstat[CSTAT_UNSPENT].tip.num_lines++] = msg->get("Unspent attribute points");

}


/**
 * Color-coding for positive/negative/no bonus
 */
int MenuCharacter::bonusColor(int stat) {
	if (stat > 0) return FONT_GREEN;
	if (stat < 0) return FONT_RED;
	return FONT_WHITE;
}

void MenuCharacter::logic() {
	if (!visible) return;

	if (closeButton->checkClick()) {
		visible = false;
	}

	// upgrade buttons
	for (int i=0; i<4; i++) {
		upgradeButton[i]->enabled = false;
	}

	int spent = stats->physical_character + stats->mental_character + stats->offense_character + stats->defense_character -4;
	int max_spendable_stat_points = 16;
	skill_points = stats->level - spent;

	if (spent < stats->level && spent < max_spendable_stat_points) {
		if (stats->physical_character < 5) upgradeButton[0]->enabled = true;
		if (stats->mental_character  < 5) upgradeButton[1]->enabled = true;
		if (stats->offense_character < 5) upgradeButton[2]->enabled = true;
		if (stats->defense_character < 5) upgradeButton[3]->enabled = true;
	}

	if (upgradeButton[0]->checkClick()) physical_up = true;
	if (upgradeButton[1]->checkClick()) mental_up = true;
	if (upgradeButton[2]->checkClick()) offense_up = true;
	if (upgradeButton[3]->checkClick()) defense_up = true;

	statList->checkClick();

	// TODO: this doesn't need to be done every frame. Only call this when something has updated
	refreshStats();
}



void MenuCharacter::render() {
	if (!visible) return;

	SDL_Rect src;
	SDL_Rect dest;

	// background
	src.x = 0;
	src.y = 0;
	dest.x = window_area.x;
	dest.y = window_area.y;
	src.w = dest.w = 320;
	src.h = dest.h = 416;
	SDL_BlitSurface(background, &src, screen, &dest);

	// close button
	closeButton->render();

	// title
	labelCharacter->render();

	// labels and values
	for (int i=0; i<CSTAT_COUNT; i++) {
		cstat[i].label->render();
		cstat[i].value->render();
	}

	// upgrade buttons
	for (int i=0; i<4; i++) {
		if (upgradeButton[i]->enabled) upgradeButton[i]->render();
	}

	statList->render();
}


/**
 * Display various mouseovers tooltips depending on cursor location
 */
TooltipData MenuCharacter::checkTooltip() {

	for (int i=0; i<CSTAT_COUNT; i++) {
		if (isWithin(cstat[i].hover, inpt->mouse) && cstat[i].tip.num_lines > 0)
			return cstat[i].tip;
	}

	TooltipData tip;
	tip.num_lines = 0;
	return tip;
}

/**
 * User might click this menu to upgrade a stat.  Check for this situation.
 * Return true if a stat was upgraded.
 */
bool MenuCharacter::checkUpgrade() {

	Point mouse;
	mouse.x = inpt->mouse.x;
	mouse.y = inpt->mouse.y;

	int spent = stats->physical_character + stats->mental_character + stats->offense_character + stats->defense_character -4;
	int max_spendable_stat_points = 16;
	skill_points = stats->level - spent;

	// check to see if there are skill points available
	if (spent < stats->level && spent < max_spendable_stat_points) {

		// check mouse hotspots

		// physical
		if (physical_up) {
			stats->physical_character++;
			stats->recalc(); // equipment applied by MenuManager
			newPowerNotification = true; //TODO: Only show if a NEW power is unlocked...
			physical_up = false;
			return true;
		}
		// mental
		else if (mental_up) {
			stats->mental_character++;
			stats->recalc(); // equipment applied by MenuManager
			newPowerNotification = true;
			mental_up = false;
			return true;
		}
		// offense
		else if (offense_up) {
			stats->offense_character++;
			stats->recalc(); // equipment applied by MenuManager
			newPowerNotification = true;
			offense_up = false;
			return true;
		}
		// defense
		else if (defense_up) {
			stats->defense_character++;
			stats->recalc(); // equipment applied by MenuManager
			newPowerNotification = true;
			defense_up = false;
			return true;
		}
	}

	return false;
}

MenuCharacter::~MenuCharacter() {
	SDL_FreeSurface(background);
	delete closeButton;

	delete labelCharacter;
	for (int i=0; i<CSTAT_COUNT; i++) {
		delete cstat[i].label;
		delete cstat[i].value;
	}
	for (int i=0; i<4; i++) {
		delete upgradeButton[i];
	}
	delete statList;
}
