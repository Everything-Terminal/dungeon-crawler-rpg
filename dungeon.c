#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// --- ANSI Colors ---
#define RESET    "\033[0m"
#define BOLD     "\033[1m"
#define RED      "\033[31m"
#define GREEN    "\033[32m"
#define YELLOW   "\033[33m"
#define BLUE     "\033[34m"
#define MAGENTA  "\033[35m"
#define CYAN     "\033[36m"

// --- Game Constants ---
#define MAX_ITEMS 10
#define MAX_ROOMS 8
#define MAX_DESC 200

// --- Data Structures ---

typedef struct {
  char name[50];
  char description[MAX_DESC];
  int heal_amount;
  int damage_bonus;
  int value;
} Item;

typedef struct {
  char name[50];
  int hp;
  int max_hp;
  int attack;
  int defense;
  int gold;
  int level;
  int experience;
  Item inventory[MAX_ITEMS];
  int inventory_count;
  int current_room;
} Player;

typedef struct {
  char name[50];
  char description[MAX_DESC];
  int north, south, east, west; // Room connections (-1 = no exit)
  Item items[MAX_ITEMS];
  int item_count;
  int enemy_hp;
  char enemy_name[50];
  int enemy_attack;
  int cleared; // 1 if enemy defeated
} Room;

 // --- Global Game State ---
Player player;
Room rooms[MAX_ROOMS];

// --- Function Prototypes ---
void init_game();
void init_rooms();
void display_welcome();
void display_stats();
void display_room();
void display_inventory();
void move_player(int direction);
void pickup_item();
void use_item();
void attack_enemy();
void check_death();
void check_victory();
void clear_input_buffer();
int get_input();

int main() {
  srand(time(NULL));
  init_game();

  int playing = 1;

  while (playing) {
    display_room();
    display_stats();

    printf(CYAN "\n What do you want to do?\n" RESET);
    printf(" " BOLD "[1]" RESET " Move North\n");
    printf(" " BOLD "[2]" RESET " Move South\n");
    printf(" " BOLD "[3]" RESET " Move East\n");
    printf(" " BOLD "[4]" RESET " Move West\n");
    printf(" " BOLD "[5]" RESET " Pick up item\n");
    printf(" " BOLD "[6]" RESET " Use item\n");
    printf(" " BOLD "[7]" RESET " View inventory\n");
    printf(" " BOLD "[8]" RESET " Attack enemy\n");
    printf(" " BOLD "[9]" RESET " Quit\n");
    printf(CYAN " Enter choice: " RESET);

    int choice = get_input();

    switch (choice) {
      case 1: move_player(0); break; // North
      case 2: move_player(1); break; // south 
      case 3: move_player(2); break; // East;
      case 4: move_player(3); break; // West 
      case 5: pickup_item(); break;
      case 6: use_item(); break;
      case 7: display_inventory(); break;
      case 8: attack_enemy(); break;
      case 9:
              printf(RED "\n Thanks for playing!\n" RESET);
              playing = 0;
              break;
      default:
              printf(YELLOW "\n Invalid choice!\n" RESET);
    }

    check_death();
    check_victory();

    if (player.hp <= 0) {
      playing = 0;
    }
  }

  return 0;
}

void init_game() {
  // Initialize player
  strcpy(player.name, "Hero");
  player.hp = 100;
  player.max_hp = 100;
  player.attack = 15;
  player.defense = 5;
  player.gold = 20;
  player.level = 1;
  player.experience = 0;
  player.inventory_count = 0;
  player.current_room = 0;

  // Give player a starting potion
  strcpy(player.inventory[0].name, "Health Potion");
  strcpy(player.inventory[0].description, "Restore 30 HP");
  player.inventory[0].heal_amount = 30;
  player.inventory[0].damage_bonus = 0;
  player.inventory[0].value = 10;
  player.inventory_count = 1;

  init_rooms();
}

void init_rooms() {
  // Room 0: Entrance Hall
  strcpy(rooms[0].name, "Entrance Hall");
  strcpy(rooms[0].description, "You stand in a dimly lit stone hallway.Torches flicker on the walls, casting dancing shadows. The air is cold and damp.");
  rooms[0].north = 1; rooms[0].south = -1; rooms[0].east = 2; rooms[0].west = -1;
  rooms[0].item_count = 0;
  rooms[0].enemy_hp = 0;
  rooms[0].cleared = 1;

  // Room 1: Guard Room (has enemy)
  strcpy(rooms[1].name, "Guard Room");
  strcpy(rooms[1].description, "A small chamber with broken furniture. A goblin guard snarls at you, clutching a rusty dagger!");
  rooms[1].north = -1; rooms[1].south = 0; rooms[1].east = 3; rooms[1].west = -1;
  rooms[1].item_count = 0;
  strcpy(rooms[1].enemy_name, "Goblin Guard");
  rooms[1].enemy_hp = 30;
  rooms[1].enemy_attack = 8;
  rooms[1].cleared = 0;

  // Room 2: Storage Room (has items)
  strcpy(rooms[2].name, "Storage Room");
  strcpy(rooms[2].description, "Dusty crates and barrels line the walls. You spot something shiny among the debris");
  rooms[2].north = 3; rooms[2].south = -1; rooms[2].east = -1; rooms[2].west = 0;

  strcpy(rooms[2].items[0].name, "Iron Sword");
  strcpy(rooms[2].items[0].description, "A sturdy blade (+5 attack)");
  rooms[2].items[0].heal_amount = 0;
  rooms[2].items[0].damage_bonus = 5;
  rooms[2].items[0].value = 25;

  strcpy(rooms[2].items[1].name, "Health Potion");
  strcpy(rooms[2].items[1].description, "Restore 30 HP");
  rooms[2].items[1].heal_amount = 30;
  rooms[2].items[1].damage_bonus = 0;
  rooms[2].items[1].value = 10;
  rooms[2].item_count = 2;
  rooms[2].enemy_hp = 0;
  rooms[2].cleared = 1;

  // Room 3: Dark Corridor
  strcpy(rooms[3].name, "Dark Corridor");
  strcpy(rooms[3].description, "A narrow passage stretches before you. Strange scratching sounds echo from the shadows");
  rooms[3].north = 4; rooms[3].south = 2; rooms[3].east = -1; rooms[3].west = 1;
  rooms[3].item_count = 0;
  strcpy(rooms[3].enemy_name, "Giant Rat");
  rooms[3].enemy_hp = 20;
  rooms[3].enemy_attack = 6;
  rooms[3].cleared = 0;

  // Room 4: Treasure Chamber
  strcpy(rooms[4].name, "Treasure Chamber");
  strcpy(rooms[4].description, "Gold coins and jewels glitter in a stone chest! But a massive creature guards it too...");
  rooms[4].north = -1; rooms[4].south = 3; rooms[4].east = 5; rooms[4].west = -1;

  strcpy(rooms[4].items[0].name, "Gold Coins");
  strcpy(rooms[4].items[0].description, "A pile of shiny gold (+50 gold)");
  rooms[4].items[0].heal_amount = 0;
  rooms[4].items[0].damage_bonus = 0;
  rooms[4].items[0].value = 50;
  rooms[4].item_count = 1;

  strcpy(rooms[4].enemy_name, "Orc Warrior");
  rooms[4].enemy_hp = 50;
  rooms[4].enemy_attack = 12;
  rooms[4].cleared = 0;

  // Room 5: Dragon's Lair (BOSS)
  strcpy(rooms[5].name, "Dragon's Lair");
  strcpy(rooms[5].description, "A massive cavern filled with treasure. A fearsome dragon awakens, smoke curling from its nostrils!");
  rooms[5].north = -1; rooms[5].south = -1; rooms[5].east = -1; rooms[5].west = 4;
  rooms[5].item_count = 0;
  strcpy(rooms[5].enemy_name, "Ancient Dragon");
  rooms[5].enemy_hp = 100;
  rooms[5].enemy_attack = 20;
  rooms[5].cleared = 0;
}

void display_welcome() {
  printf(CYAN BOLD);
  printf("\n  ╔════════════════════════════════════════════╗\n");
    printf("  ║      🗡️  DUNGEON CRAWLER RPG  🗡️           ║\n");
    printf("  ╚════════════════════════════════════════════╝\n");
  printf(RESET);
  printf("\n "YELLOW "Welcome, brave adventurer!\n" RESET);
  printf(" You have entered the " BOLD "Dungeon of Shadows" RESET YELLOW ".\n");
  printf(" Defeat the " RED BOLD "Ancient Dragon" RESET YELLOW " to claim victory!\n\n");
}

void display_stats() {
  printf("\n " BLUE "───────────────────────────────────────────────────────────\n" RESET);
  printf(" " BOLD "%s" RESET " | ", player.name);
  printf("Level: " GREEN "%d" RESET " | ", player.level);
  printf("HP: %s%d/%d" RESET " | ",
      player.hp < 30 ? RED : GREEN, player.hp, player.max_hp);
  printf("ATK: " RED "%d" RESET " | ", player.attack);
  printf("DEF: " BLUE "%d" RESET " | ", player.defense);
  printf("GOLD: " YELLOW "%d\n" RESET, player.gold);
  printf(" " BLUE "───────────────────────────────────────────────────────────\n" RESET);
}

void display_room() {
  Room *room = &rooms[player.current_room];

   printf(CYAN BOLD "\n  ╔════════════════════════════════════════════╗\n");
    printf("  ║  %-40s  ║\n", room->name);
    printf("  ╚════════════════════════════════════════════╝\n" RESET);

  printf("\n " YELLOW "%s\n" RESET, room->description);

  // Show exits
  printf("\n " BLUE "Exits:" RESET);
  if (room->north != -1) printf(" [North]");
  if (room->south != -1) printf(" [South]");
  if (room->east != -1) printf(" [East]");
  if (room->west != -1) printf(" [West]");
  printf("\n");

  // Show items
  if (room->item_count > 0) {
    printf("\n " GREEN "Items here:\n" RESET);
    for (int i = 0; i < room->item_count; i++) {
      printf("   • %s\n", room->items[i].name);
    }
  }

  // Show enemy
  if (!room->cleared && room->enemy_hp > 0) {
    printf("\n " RED BOLD "⚔️ ENEMY: %s (HP: %d, ATK: %d)\n" RESET,
        room->enemy_name, room->enemy_hp, room->enemy_attack);
  } else if (room->cleared && room->enemy_hp > 0) {
    printf("\n " GREEN "✓ Enemy defeated!\n" RESET);
  }
}

void display_inventory() {
  printf(CYAN BOLD "\n ═══ Inventory ═══\n" RESET);

  if (player.inventory_count == 0) {
    printf(YELLOW " Your inventory is empty.\n" RESET);
    return;
  }

  for (int i = 0; i < player.inventory_count; i++) {
    printf(" " BOLD "[%d]" RESET " %s - %s\n",
        i + 1, player.inventory[i].name, player.inventory[i].description);
  }
}

void move_player(int direction) {
  Room *room = &rooms[player.current_room];
  int new_room = -1;

  switch (direction) {
    case 0: new_room = room->north; break;
    case 1: new_room = room->south; break;
    case 2: new_room = room->east; break;
    case 3: new_room = room->west; break;
  }

  if (new_room == -1) {
    printf(RED "\n You can't go that way!\n" RESET);
  } else {
    player.current_room = new_room;
    printf(GREEN "\n You move to %s.\n" RESET, rooms[new_room].name);
  }
}

void pickup_item() {
  Room *room = &rooms[player.current_room];

  if (room->item_count == 0) {
    printf(YELLOW "\n There are no items to pick up here.\n" RESET);
    return;
  }

  if (player.inventory_count >= MAX_ITEMS) {
    printf(RED "\n Your inventory is full!\n" RESET);
    return;
  }

  printf("\n Which item do you want to pick up?\n");
  for (int i = 0; i < room->item_count; i++) {
    printf(" " BOLD "[%d]" RESET " %s\n", i + 1, room->items[i].name);
  }
  printf(CYAN " Enter number: " RESET);

  int choice = get_input();

  if (choice >= 1 && choice <= room->item_count) {
    player.inventory[player.inventory_count] = room->items[choice - 1];
    player.inventory_count++;

    // Special handling for gold
    if (strcmp(room->items[choice - 1].name, "Gold Coins") == 0) {
      player.gold += room->items[choice -1].value;
      printf(GREEN "\n You picked up %s (+%d gold)!\n" RESET,
          player.inventory[player.inventory_count -1].name,
          room->items[choice-1].value);
      player.inventory_count--; //Don't keep gold in inventory
    } else {
      printf(GREEN "\n You picked up %s!\n" RESET,
          player.inventory[player.inventory_count - 1].name);
    }

    // Remove item from room
    for (int i = choice -1; i < room->item_count -1; i++) {
      room->items[i] = room->items[i + 1];
    }
    room->item_count--;
  } else {
    printf(RED "\n Invalid choice!\n" RESET);
  }
}

void use_item() {
  if (player.inventory_count == 0) {
    printf(YELLOW "\n Your inventory is empty!\n" RESET);
    return;
  }

  display_inventory();
  printf(CYAN "\n Which item do you want to use? (1-%d): " RESET, player.inventory_count);

  int choice = get_input();

  if (choice >= 1 && choice <= player.inventory_count) {
    Item *item = &player.inventory[choice - 1];

    if (item->heal_amount > 0) {
      player.hp += item->heal_amount;
      if (player.hp > player.max_hp) player.hp = player.max_hp;
      printf(GREEN "\n You used %s! HP restored to %d.\n" RESET,
          item->name, player.hp);
    } else if (item->damage_bonus > 0) {
      player.attack += item->damage_bonus;
      printf(GREEN "\n You equipped %s! Attack increased to %d.\n" RESET,
          item->name, player.attack);
    }

    // Remove item from inventory
    for (int i = choice - 1; i < player.inventory_count - 1; i++) {
      player.inventory[i] = player.inventory[i + 1];
    }
    player.inventory_count--;
  } else {
    printf(RED "\n Invalid choice!\n" RESET);
  }
}

void attack_enemy() {
  Room *room = &rooms[player.current_room];

  if (room->cleared || room->enemy_hp <= 0) {
    printf(YELLOW "\n There's no enemy to attack here!\n" RESET);
    return;
  }

  // Player attacks
  int player_damage = player.attack + (rand() %5);
  room->enemy_hp -= player_damage;
  printf(RED "\n ⚔️ You attack %s for %d damage!\n" RESET,
      room->enemy_name, player_damage);

  if (room->enemy_hp <= 0) {
    printf(GREEN BOLD "\n ✓  You defeated the %s!\n" RESET, room->enemy_name);
    room->cleared = 1;

    // Award experience and gold
    int exp_gain = 20 + (rand() % 15);
    int gold_gain = 10 + (rand() % 20);
    player.experience += exp_gain;
    player.gold += gold_gain;
    printf(YELLOW " +%d EXP, +%d Gold!\n" RESET, exp_gain, gold_gain);

    // level up check
    if (player.experience >= player.level  * 50) {
      player.level++;
      player.max_hp += 20;
      player.hp = player.max_hp;
      player.attack += 3;
      player.defense += 2;
      printf(GREEN BOLD "\n ★  LEVEL UP! You are now level %d!\n" RESET, player.level);
      printf(" HP: %d | ATK: %d | DEF: %d\n", player.max_hp, player.attack, player.defense);
    }
  } else {
    // Enemy attacks back
    int enemy_damage = room->enemy_attack + (rand() % 4) - player.defense;
    if (enemy_damage < 1) enemy_damage = 1;
    player.hp -= enemy_damage;
    printf(RED " %s attacks you for %d damage!\n" RESET, room->enemy_name, enemy_damage);
  }
}

void check_death() {
  if (player.hp <= 0) {
    printf(RED BOLD);
    printf("\n ╔══════════════════════════════════════════════════╗\n");
    printf(" ║                💀 GAME OVER 💀                    ║\n");
    printf(" ╚═══════════════════════════════════════════════════╝\n");
    printf(RESET);
    printf("\n " YELLOW "You have fallen in the dungeon...\n" RESET);
    printf(" Final Level: %d | Gold: %d\n\n", player.level, player.gold);
  }
}

void check_victory() {
  // Check if dragon is defeated (room 5)
  if (rooms[5].cleared && rooms[5].enemy_hp <= 0) {
    printf(GREEN BOLD);
    printf("\n ╔═══════════════════════════════════════════════════╗\n");
    printf(" ║                 🏆 VICTORY 🏆                      ║\n");
    printf(" ╚════════════════════════════════════════════════════╝\n");
    printf(RESET);
    printf("\n " YELLOW "You have defeated the Ancient Dragon!\n" RESET);
    printf(" The dungeon is yours! Treasure beyond imagination awaits!\n");
    printf(" Final Level: %d | Gold: %d\n\n", player.level, player.gold);
    exit(0);
  }
}

void clear_input_buffer() {
  int c;
  while ((c = getchar()) != '\n' && c != EOF);
}

int get_input() {
  int input;
  if (scanf("%d", &input) != 1) {
    clear_input_buffer();
    return -1;
  }
  clear_input_buffer();
  return input;
}


