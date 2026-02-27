#include "dragon.h"

////////////////////////////////////////////////////////////////////////
/// STUDENT'S ANSWER BEGINS HERE
/// Complete the following functions
/// DO NOT modify any parameters in the functions.
////////////////////////////////////////////////////////////////////////

// 3.1
MapElement::MapElement(ElementType type, int in_req_dmg): type(type), req_dmg(in_req_dmg) {}

MapElement::~MapElement() {}

ElementType MapElement::getType() const {
    return type;
}

int MapElement::getReqDmg() const {
    return req_dmg;
}


// 3.2
Map::Map(int rows, int cols, int num_obst, Position *obst, int num_gro_obst, Position *gro_obst): num_rows(rows), num_cols(cols){

    grid = new MapElement**[num_rows];

    for (int i = 0; i < num_rows; ++i) {
        grid[i] = new MapElement*[num_cols];
        for (int j = 0; j < num_cols; ++j) {
            grid[i][j] = new MapElement(PATH);  
        }
    }

    for (int i = 0; i < num_obst; ++i) {
        int r = obst[i].getRow();
        int c = obst[i].getCol();
        delete grid[r][c];  
        grid[r][c] = new MapElement(OBSTACLE); 
    }

    for (int i = 0; i < num_gro_obst; ++i) {
        int r = gro_obst[i].getRow();
        int c = gro_obst[i].getCol();
        int dmg = (r * 257 + c * 139 + 89) % 900 + 1;
        delete grid[r][c];
        grid[r][c] = new MapElement (GROUND_OBSTACLE, dmg); 
    }
}

Map::~Map() {
    for (int i = 0; i < num_rows; ++i) {
        for (int j = 0; j < num_cols; ++j) {
            delete grid[i][j];  
        }
        delete[] grid[i];  
    }
    delete[] grid;  
}

bool Map::isValid(const Position & pos, const MovingObject * obj) const {
    int r = pos.getRow();
    int c = pos.getCol();
    
    if (r < 0 || r >= num_rows || c < 0 || c >= num_cols) {
        return false;  
    }

    ElementType type = grid[r][c]->getType();
    return type != OBSTACLE; 
}

int Map::getNumRows() const {
    return num_rows;
}

int Map::getNumCols() const {
    return num_cols;
}

bool Map::isPath(const Position & pos) const {
    if (pos.getRow() < 0 || pos.getRow() >= num_rows || pos.getCol() < 0 || pos.getCol() >= num_cols) return false;
    return grid[pos.getRow()][pos.getCol()]->getType() == PATH;
}

bool Map::isObstacle(const Position & pos) const {
    if (pos.getRow() < 0 || pos.getRow() >= num_rows || pos.getCol() < 0 || pos.getCol() >= num_cols) return false;
    return grid[pos.getRow()][pos.getCol()]->getType() == OBSTACLE;
}

bool Map::isGroundObstacle(const Position & pos) const {
    if (pos.getRow() < 0 || pos.getRow() >= num_rows || pos.getCol() < 0 || pos.getCol() >= num_cols) return false;
    return grid[pos.getRow()][pos.getCol()]->getType() == GROUND_OBSTACLE;
}


// 3.3
Position::Position(int r, int c) : r_(r), c_(c) {}
Position::Position(const std::string& s) {
    r_ = c_ = 0;

    std::string cleaned = s;
    if (!cleaned.empty() && cleaned.front() == '(') cleaned.erase(0, 1);
    if (!cleaned.empty() && cleaned.back() == ')') cleaned.pop_back();

    size_t comma = cleaned.find(',');
    if (comma == std::string::npos) return;

    std::string r_str = cleaned.substr(0, comma);
    std::string c_str = cleaned.substr(comma + 1);

    if (!r_str.empty() && !c_str.empty()) {
        r_ = std::stoi(r_str);
        c_ = std::stoi(c_str);
    }
}


int Position::getRow() const {
    return r_;
}

int Position::getCol() const {
    return c_;
}

void Position::setRow(int r) {
    r_ = r;
}

void Position::setCol(int c) {
    c_ = c;
}

string Position::str() const {
    return "(" + to_string(r_) + "," + to_string(c_) + ")";
}

bool Position::isEqual(int in_r, int in_c) const {
    return r_ == in_r && c_ == in_c;
}

const Position Position::npos = Position(-1, -1);


bool operator!=(const Position& lhs, const Position& rhs) {
    return lhs.getRow() != rhs.getRow() || lhs.getCol() != rhs.getCol();
}

bool operator==(const Position& lhs, const Position& rhs) {
    return lhs.getRow() == rhs.getRow() && lhs.getCol() == rhs.getCol();
}

int Position::manhattanDistance(const Position &other) const {
    return abs(r_ - other.getRow()) + abs(c_ - other.getCol());
}




// 3.4
MovingObject::MovingObject(int index, const Position& pos, Map* map, const string& name) 
: index(index), pos(pos), map(map), name(name) {}
MovingObject::~MovingObject() {}

Position MovingObject::getCurrentPosition() const {
    return pos;
}


string oppositeDirection(char direction) {
    switch (direction) {
        case 'U': return "D";
        case 'D': return "U";
        case 'L': return "R";
        case 'R': return "L";
        default: return "";
    }
}


// 3.5
Warrior::Warrior(int index, const Position& pos, Map* map, const string& name, int init_hp, int init_damage)
    : MovingObject(index, pos, map, name) {

    this->hp = std::max(0, std::min(init_hp, 500));
    this->damage = std::max(0, std::min(init_damage, 900));

    this->bag = nullptr;
}

Warrior::~Warrior() {
    delete bag;  
}

int Warrior::getHp() {
    return hp;
}

int Warrior::getDamage() {
    return damage;
}

void Warrior::setHp(int new_hp) {
    if (new_hp < 0) new_hp = 0;
    else if (new_hp > 500) new_hp = 500;
    hp = new_hp;
}

void Warrior::setDamage(int new_damage) {
    if (new_damage < 0) new_damage = 0;
    else if (new_damage > 900) new_damage = 900;
    damage = new_damage;
}

BaseBag* Warrior::getBag() const {
    return bag;
}

bool Warrior::isDragonLord() const {
    return false;  
}


//3.6
FlyTeam::FlyTeam(int index, const string& moving_rule, const Position& pos, Map* map, int init_hp, int init_damage)
    : Warrior(index, pos, map, (index == 1 ? "FlyTeam1" : "FlyTeam2"), init_hp, init_damage),
      moving_rule(moving_rule),
      moving_index(0) {
}

Position FlyTeam::getNextPosition() {
    if (moving_rule.empty()) return Position::npos;

    char direction = moving_rule[moving_index];
    int row_offset = 0;
    int col_offset = 0;

    if (direction == 'L') {
        col_offset = -1;
    } else if (direction == 'R') {
        col_offset = 1;
    } else if (direction == 'U') {
        row_offset = -1;
    } else if (direction == 'D') {
        row_offset = 1;
    } else {
        return Position::npos; 
    }

    int nextRow = pos.getRow() + row_offset;
    int nextCol = pos.getCol() + col_offset;
    Position nextPos(nextRow, nextCol);

    if (!map->isValid(nextPos, this)) {
        return Position::npos;
    }
    return nextPos;
}

Position FlyTeam::getRereversePosition() const {
    if (moving_rule.empty()) return Position::npos;

    char direction = moving_rule[moving_index];
    int row_offset = 0;
    int col_offset = 0;

    if (direction == 'L') {
        col_offset = 1;  
    } else if (direction == 'R') {
        col_offset = -1; 
    } else if (direction == 'U') {
        row_offset = 1;  
    } else if (direction == 'D') {
        row_offset = -1; 
    } else {
        return Position::npos; 
    }

    Position nextPos(pos.getRow() + row_offset, pos.getCol() + col_offset);
    return nextPos;
}


void FlyTeam::move() {
    if (hp <= 1) {
        pos = Position::npos;
        return;
    }

    cout << "MSG: " << getName() << " moved\n";

    if (moving_rule.empty()) return;

    moving_index %= (int)moving_rule.size();
    char dir = moving_rule[moving_index];

    Position next_pos = getNextPosition();
    if (map->isValid(next_pos, this) && map->isPath(next_pos)) {
        pos = next_pos;
    } else {
        Position cur = getCurrentPosition();
        Position rev = getRereversePosition();

        cout << "MSG: " << getName() << " at " << cur.str()
             << " got blocked when moving " << dir
             << " to " << next_pos.str() << endl;

        if (map->isValid(rev, this) && map->isPath(rev)) {
            cout << "MSG: " << getName()
                 << " moved to the opposite direction "
                 << oppositeDirection(dir) << " to " << rev.str() << endl;
            pos = rev;
        } else {
            cout << "MSG: " << getName()
                 << " got blocked both sides, stay at "
                 << cur.str() << endl;
        }
    }

    moving_index = (moving_index + 1) % (int)moving_rule.size();
}

string FlyTeam::str() const {
    int team_id = index < 1 ? 1 : (index > 2 ? 2 : index);
    return "FlyTeam" + std::to_string(team_id)
         + "[index=" + std::to_string(index)
         + ";pos=" + pos.str()
         + ";moving_rule=" + moving_rule + "]";
}

bool FlyTeam::attack(DragonLord* dragon) {
    if (!dragon) return false;

    return getCurrentPosition().isEqual(dragon->getPosition().getRow(),dragon->getPosition().getCol());
}

bool FlyTeam::isDragonLord() const {
    return false;  
}

bool FlyTeam::isSmartDragon() const {
    return false;  
}


// 3.7
GroundTeam::GroundTeam(int index, const string& moving_rule, const Position& pos, Map* map, int init_hp, int init_damage)
    : Warrior(index, pos, map, "GroundTeam", init_hp, init_damage),
      moving_rule(moving_rule),
      moving_index(0),
      trap_turns(3),
      isTrapped(false),
      trap_duration(0) {
}

Position GroundTeam::getNextPosition() {
    if (moving_rule.empty()) return Position::npos;

    char direction = moving_rule[moving_index];
    int row_offset = 0;
    int col_offset = 0;

    if (direction == 'L') {
        col_offset = -1;
    } else if (direction == 'R') {
        col_offset = 1;
    } else if (direction == 'U') {
        row_offset = -1;
    } else if (direction == 'D') {
        row_offset = 1;
    } else {
        return Position::npos; 
    }

    int nextRow = pos.getRow() + row_offset;
    int nextCol = pos.getCol() + col_offset;
    Position nextPos(nextRow, nextCol);

    if (!map->isValid(nextPos, this)) {
        return Position::npos;
    }

    return nextPos;
}

Position GroundTeam::getRereversePosition() const {
    if (moving_rule.empty()) return Position::npos;

    char direction = moving_rule[moving_index];
    int row_offset = 0;
    int col_offset = 0;

    if (direction == 'L') {
        col_offset = 1;  
    } else if (direction == 'R') {
        col_offset = -1; 
    } else if (direction == 'U') {
        row_offset = 1;  
    } else if (direction == 'D') {
        row_offset = -1; 
    } else {
        return Position::npos; 
    }

    Position nextPos(pos.getRow() + row_offset, pos.getCol() + col_offset);
    return nextPos;
}

void GroundTeam::move() {
    if (hp <= 1) {
        pos = Position::npos;
        return;
    }

    cout << "MSG: " << getName() << " moved\n";

    if (moving_rule.empty()) return;

    moving_index %= (int)moving_rule.size();
    char dir = moving_rule[moving_index];

    Position next_pos = getNextPosition();
    if (map->isValid(next_pos, this) && canMoveTo(next_pos)) {
        pos = next_pos;
    } else {
        Position cur = getCurrentPosition();
        Position rev = getRereversePosition();

        cout << "MSG: " << getName() << " at " << cur.str()
             << " got blocked when moving " << dir
             << " to " << next_pos.str() << endl;

        if (map->isValid(rev, this) && canMoveTo(rev)) {
            cout << "MSG: " << getName()
                 << " moved to the opposite direction "
                 << oppositeDirection(dir) << " to " << rev.str() << endl;
            pos = rev;
        } else {
            cout << "MSG: " << getName()
                 << " got blocked both sides, stay at "
                 << cur.str() << endl;
        }
    }

    moving_index = (moving_index + 1) % (int)moving_rule.size();
}

string GroundTeam::str() const {
    return "GroundTeam[index=" + std::to_string(index) + ";pos=" + pos.str() + ";moving_rule=" + moving_rule + "]";
}

bool GroundTeam::swapPosition() {
    Position cur = getCurrentPosition();
    Position swapped(cur.getCol(), cur.getRow()); 

    if (canMoveTo(swapped)) {
        pos = swapped;
        return true;
    }

    int dx[] = {-1, 0, 1, 0};
    int dy[] = {0, 1, 0, -1};

    for (int i = 0; i < 4; ++i) {
        Position adjacent(cur.getRow() + dx[i], cur.getCol() + dy[i]);
        if (canMoveTo(adjacent)) {
            pos = adjacent;
            return true;
        }
    }

    setHp(1); 
    return false;
}

bool GroundTeam::trap(DragonLord* dragonlord) {
    if (isTrapped) {
        --trap_duration;

        if (trap_duration == 0) {
            setIsTrapped(false);
            trap_turns = 3;

            if (swapPosition()) {
                cout << "MSG: DragonLord escaped the trap! GroundTeam transferred to "
                     << getCurrentPosition().str() << endl;
            } else {
                cout << "MSG: DragonLord escaped the trap! Failed to move GroundTeam - eliminated!" << endl;
            }

            return true;
        }

        return true; 
    }

    Position dragon_pos = dragonlord->getPosition();
    Position team_pos = getCurrentPosition();

    if (team_pos.isEqual(dragon_pos.getRow(), dragon_pos.getCol())) {
        cout << "MSG: GroundTeam trapped DragonLord for " << trap_turns << " turns" << endl;
        setIsTrapped(true);
        trap_duration = trap_turns;
        return true;
    }

    return false;
}

bool GroundTeam::canMoveTo(const Position& p) const {
    if (!map->isValid(p, this)) return false;
    if (map->isPath(p)) return true;
    
    if (map->isGroundObstacle(p)) {
        int hash = (p.getRow() * 257 + p.getCol() * 139 + 89) % 900 + 1;
        return hash < damage;
    }

    return false;
}

int GroundTeam::getTrapTurns() const {
    return trap_turns;
}

void GroundTeam::setTrapTurns(int turns) {
    trap_turns = turns;
}







//3.8
DragonLord::DragonLord(int index, const Position& init_pos, Map* map,
                       FlyTeam* flyteam1, FlyTeam* flyteam2, GroundTeam* ground_team)
    : MovingObject(index, init_pos, map, "DragonLord"),
      flyteam1(flyteam1),
      flyteam2(flyteam2),
      hp(500),
      damage(900) {
    (void)ground_team; // tránh warning unused
}

Position DragonLord::getNextPosition() {
    const Position& pos1 = flyteam1->getCurrentPosition();
    const Position& pos2 = flyteam2->getCurrentPosition();

    int deltaRow = std::abs(pos1.getRow() - pos2.getRow());
    int deltaCol = std::abs(pos1.getCol() - pos2.getCol());

    return Position(deltaRow, deltaCol);
}

int DragonLord::manhattanDistance(const Position pos1, const Position pos2) const {
    return abs(pos1.getRow() - pos2.getRow()) + abs(pos1.getCol() - pos2.getCol());
}

void DragonLord::move() {
    cout<< "MSG: " << getName() << " moved\n";
    Position next_pos = getNextPosition();
    if (map->isValid(next_pos, this)) {
        pos = next_pos;
    }
}

string DragonLord::str() const {
    return "DragonLord[index=" + std::to_string(index) + ";pos=" + pos.str() + "]";
}

int DragonLord::getHp() {
    return hp;
}

int DragonLord::getDamage() {
    return damage;
}

void DragonLord::setHp(int new_hp) {
    if (new_hp < 0) new_hp = 0;
    else if (new_hp > 500) new_hp = 500;
    hp = new_hp;
}

void DragonLord::setDamage(int new_damage) {
    if (new_damage < 0) new_damage = 0;
    else if (new_damage > 900) new_damage = 900;
    damage = new_damage;
}

bool DragonLord::isDragonLord() const {
    return true;  
}

bool DragonLord::isSmartDragon() const {
    return false;
}

Position DragonLord::getPosition() const {
    return pos;
}

int DragonLord::getIndex() const {
    return index;
}


// 3.9
ArrayMovingObject::ArrayMovingObject(int capacity)
    : arr_mv_objs(nullptr), count(0), capacity(capacity < 0 ? 0 : capacity) {
    arr_mv_objs = (this->capacity > 0) ? new MovingObject*[this->capacity] : nullptr;
    for (int i = 0; i < this->capacity; ++i) arr_mv_objs[i] = nullptr;
}

ArrayMovingObject::~ArrayMovingObject() {
    delete[] arr_mv_objs;

    arr_mv_objs = nullptr;
    count = 0;
    capacity = 0;
}

bool ArrayMovingObject::isFull() const {
    return count >= capacity;
}

bool ArrayMovingObject::add(MovingObject *mv_obj) {
    if (isFull() || mv_obj == nullptr) return false;

    arr_mv_objs[count++] = mv_obj;
    return true;
}

string ArrayMovingObject::str() const {
    string result = "ArrayMovingObject[";
    result += "count=" + to_string(count) + ";capacity=" + to_string(capacity) + ";";
    for (int i = 0; i < count; ++i) {
        result += arr_mv_objs[i]->str();
        if (i < count - 1) {
            result += ";";
        }
    }
    result += "]";
    return result;
}

int ArrayMovingObject::getCount() const {
    return count;
}


//3.10
Configuration::Configuration(const string& filepath)
    : map_num_rows(0), map_num_cols(0), max_num_moving_objects(0),
    num_obstacles(0), arr_obstacles(nullptr), num_ground_obstacles(0),
    arr_ground_obstacles(nullptr), flyteam1_moving_rule(""), flyteam1_init_pos(),
    flyteam1_init_hp(0), flyteam1_init_damage(0), flyteam2_moving_rule(""),
    flyteam2_init_pos(), flyteam2_init_hp(0), flyteam2_init_damage(0),
    groundteam_moving_rule(""), groundteam_init_pos(), groundteam_init_hp(0),
    groundteam_init_damage(0), dragonlord_init_pos(), num_steps(0) {

    ifstream file(filepath);
    if (!file.is_open()) {
        cerr << "Error opening configuration file: " << filepath << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;

        size_t equal_pos = line.find('=');
        if (equal_pos == string::npos) continue;

        string key = line.substr(0, equal_pos);
        string value = line.substr(equal_pos + 1);

        auto toInt = [](const string& val) { return !val.empty() ? stoi(val) : 0; };

        if (key == "MAP_NUM_ROWS") map_num_rows = toInt(value);
        else if (key == "MAP_NUM_COLS") map_num_cols = toInt(value);
        else if (key == "MAX_NUM_MOVING_OBJECTS") max_num_moving_objects = toInt(value);
        else if (key == "NUM_OBSTACLE") num_obstacles = toInt(value);
        else if (key == "ARRAY_OBSTACLE") {
            if (num_obstacles == 0) num_obstacles = countPositionsInArray(value);
            if (num_obstacles > 0) {
                arr_obstacles = new Position[num_obstacles];
                parsePositionArray(value, arr_obstacles, num_obstacles);
            }
        } else if (key == "NUM_GROUND_OBSTACLE") num_ground_obstacles = toInt(value);
        else if (key == "ARRAY_GROUND_OBSTACLE") {
            if (num_ground_obstacles == 0) num_ground_obstacles = countPositionsInArray(value);
            if (num_ground_obstacles > 0) {
                arr_ground_obstacles = new Position[num_ground_obstacles];
                parsePositionArray(value, arr_ground_obstacles, num_ground_obstacles);
            }
        } else if (key == "FLYTEAM1_MOVING_RULE") flyteam1_moving_rule = value;
        else if (key == "FLYTEAM1_INIT_POS") flyteam1_init_pos = Position(removeParentheses(value));
        else if (key == "FLYTEAM1_INIT_HP") flyteam1_init_hp = toInt(value);
        else if (key == "FLYTEAM1_INIT_DMG" || key == "FLYTEAM1_INIT_DAMAGE") flyteam1_init_damage = toInt(value);
        else if (key == "FLYTEAM2_MOVING_RULE") flyteam2_moving_rule = value;
        else if (key == "FLYTEAM2_INIT_POS") flyteam2_init_pos = Position(removeParentheses(value));
        else if (key == "FLYTEAM2_INIT_HP") flyteam2_init_hp = toInt(value);
        else if (key == "FLYTEAM2_INIT_DMG" || key == "FLYTEAM2_INIT_DAMAGE") flyteam2_init_damage = toInt(value);
        else if (key == "GROUNDTEAM_MOVING_RULE") groundteam_moving_rule = value;
        else if (key == "GROUNDTEAM_INIT_POS") groundteam_init_pos = Position(removeParentheses(value));
        else if (key == "GROUNDTEAM_INIT_HP") groundteam_init_hp = toInt(value);
        else if (key == "GROUNDTEAM_INIT_DMG" || key == "GROUNDTEAM_INIT_DAMAGE") groundteam_init_damage = toInt(value);
        else if (key == "DRAGONLORD_INIT_POS") dragonlord_init_pos = Position(removeParentheses(value));
        else if (key == "NUM_STEPS") num_steps = toInt(value);
    }

    file.close();
}


int Configuration::countPositionsInArray(const string& value) {
    if (value.size() < 2) return 0;

    string processed = value;
    if (processed.front() == '[') processed.erase(0, 1);
    if (!processed.empty() && processed.back() == ']') processed.pop_back();

    if (processed.empty()) return 0;

    int count = 1;
    for (char ch : processed) {
        if (ch == ';') ++count;
    }
    return count;
}


void Configuration::parsePositionArray(const string& value, Position* arr, int count) {
    string processed = value;
    if (!processed.empty() && processed.front() == '[') {
        processed.erase(0, 1);
    }
    if (!processed.empty() && processed.back() == ']') {
        processed.pop_back();
    }

    size_t start = 0;
    int pos_index = 0;

    while (start < processed.size() && pos_index < count) {
        size_t end = processed.find(';', start);
        if (end == string::npos) end = processed.size();

        string pos_str = processed.substr(start, end - start);

        if (!pos_str.empty() && pos_str.front() == '(') {
            pos_str.erase(0, 1);
        }
        if (!pos_str.empty() && pos_str.back() == ')') {
            pos_str.pop_back();
        }

        size_t comma_pos = pos_str.find(',');
        if (comma_pos != string::npos) {
            int row = stoi(pos_str.substr(0, comma_pos));
            int col = stoi(pos_str.substr(comma_pos + 1));
            arr[pos_index++] = Position(row, col);
        }

        start = end + 1;
    }
}

string Configuration::removeParentheses(const string& value) {
    if (value.empty()) return value;
    size_t start = (value.front() == '(') ? 1 : 0;
    size_t end = (value.back() == ')') ? value.length() - 1 : value.length();
    return value.substr(start, end - start);
}

Configuration::~Configuration() {
    delete[] arr_obstacles;
    delete[] arr_ground_obstacles;
}

string Configuration::str() const {
    string result = "Configuration[\n";
    result += "MAP_NUM_ROWS=" + to_string(map_num_rows) + "\n";
    result += "MAP_NUM_COLS=" + to_string(map_num_cols) + "\n";
    result += "MAX_NUM_MOVING_OBJECTS=" + to_string(max_num_moving_objects) + "\n";
    result += "NUM_OBSTACLE=" + to_string(num_obstacles) + "\n";

    result += "ARRAY_OBSTACLE=[";
    for (int i = 0; i < num_obstacles; ++i) {
        result += arr_obstacles[i].str();
        if (i < num_obstacles - 1) result += ";";
    }
    result += "]\n";

    result += "NUM_GROUND_OBSTACLE=" + to_string(num_ground_obstacles) + "\n";
    result += "ARRAY_GROUND_OBSTACLE=[";
    for (int i = 0; i < num_ground_obstacles; ++i) {
        result += arr_ground_obstacles[i].str();
        if (i < num_ground_obstacles - 1) result += ";";
    }
    result += "]\n";

    result += "FLYTEAM1_MOVING_RULE=" + flyteam1_moving_rule + "\n";
    result += "FLYTEAM1_INIT_POS=" + flyteam1_init_pos.str() + "\n";
    result += "FLYTEAM1_INIT_HP=" + to_string(flyteam1_init_hp) + "\n";
    result += "FLYTEAM1_INIT_DAMAGE=" + to_string(flyteam1_init_damage) + "\n";

    result += "FLYTEAM2_MOVING_RULE=" + flyteam2_moving_rule + "\n";
    result += "FLYTEAM2_INIT_POS=" + flyteam2_init_pos.str() + "\n";
    result += "FLYTEAM2_INIT_HP=" + to_string(flyteam2_init_hp) + "\n";
    result += "FLYTEAM2_INIT_DAMAGE=" + to_string(flyteam2_init_damage) + "\n";

    result += "GROUNDTEAM_MOVING_RULE=" + groundteam_moving_rule + "\n";
    result += "GROUNDTEAM_INIT_POS=" + groundteam_init_pos.str() + "\n";
    result += "GROUNDTEAM_INIT_HP=" + to_string(groundteam_init_hp) + "\n";
    result += "GROUNDTEAM_INIT_DAMAGE=" + to_string(groundteam_init_damage) + "\n";

    result += "DRAGONLORD_INIT_POS=" + dragonlord_init_pos.str() + "\n";
    result += "NUM_STEPS=" + to_string(num_steps) + "\n";
    result += "]";

    return result;
}



//3.11
SmartDragon::SmartDragon(int index, const Position & init_pos, Map * map, 
                         DragonType type, MovingObject *obj, int damage)
    : MovingObject(index, init_pos, map, "SmartDragon"), smartdragon_type(type), 
      hp(300), damage(damage), item(nullptr), target(obj), target_pos(init_pos){
         this->damage = max(this->damage, 0);
        this->damage = min(this->damage, 900);
}

Position SmartDragon::getNextPosition() {
    Position target_pos = target->getCurrentPosition();

    if (smartdragon_type == SD1 || smartdragon_type == SD2) {
        int dx[4] = {-1, 0, 1, 0}; // UP, RIGHT, DOWN, LEFT
        int dy[4] = {0, 1, 0, -1};

        Position best_pos = pos;
        int min_dist = abs(pos.getRow() - target_pos.getRow()) + abs(pos.getCol() - target_pos.getCol());

        for (int i = 0; i < 4; ++i) {
            int new_row = pos.getRow() + dx[i];
            int new_col = pos.getCol() + dy[i];
            Position next_pos(new_row, new_col);

            if (map->isValid(next_pos, this)) {
                int dist = abs(new_row - target_pos.getRow()) + abs(new_col - target_pos.getCol());
                if (dist < min_dist) {
                    min_dist = dist;
                    best_pos = next_pos;
                }
            }
        }

        return best_pos;
    }

    if (smartdragon_type == SD3) {
        return target_pos;
    }

    return pos;
}

void SmartDragon::move() {
    std::cout << "MSG: " << getName() << " moved\n";

    Position next_pos = getNextPosition();
    int row = std::max(0, next_pos.getRow());
    int col = std::max(0, next_pos.getCol());

    if (map) {
        row = std::min(row, map->getNumRows() - 1);
        col = std::min(col, map->getNumCols() - 1);
    }

    Position clamped_pos(row, col);

    if (map->isValid(clamped_pos, this) && clamped_pos != Position::npos) {
        pos = clamped_pos;
    }
}

std::string SmartDragon::str() const {
    std::string type_str = (smartdragon_type == SD1) ? "SD1" :
                           (smartdragon_type == SD2) ? "SD2" : "SD3";

    return "smartdragon[pos=" + pos.str() +
           ";type=" + type_str +
           ";tg=" + target->str() + "]";
}

int SmartDragon::getHp() {
    return hp;
}

int SmartDragon::getDamage() {
    return damage;
}

void SmartDragon::setHp(int new_hp) {
    if (new_hp < 0) hp = 0;
    else hp = new_hp;
}

void SmartDragon::setDamage(int new_damage) {
    if (new_damage < 0) damage = 0;
    else if (new_damage > 900) damage = 900;
    else damage = new_damage;
}



//3.12 and 3: 13
BaseItem::BaseItem(ItemType type, int value) : type(type), value(value) {}
BaseItem::~BaseItem() {}
BaseBag::BaseBag(int capacity)
    : capacity(capacity), count(0), owner(nullptr) {
    items = new BaseItem*[capacity];
    for (int i = 0; i < capacity; ++i) {
        items[i] = nullptr;
    }
}

BaseBag::~BaseBag() {
    for (int i = 0; i < capacity; ++i) {
        delete items[i];
    }
    delete[] items;
}
bool BaseBag::insert(BaseItem* item) {
    if (!item) return false;

    for (int i = 0; i < capacity; ++i) {
        if (!items[i]) {
            items[i] = item;
            ++count;
            return true;
        }
    }
    return false;
}
BaseItem* BaseBag::get() {
    if (!owner) return nullptr;

    for (int i = 0; i < capacity; ++i) {
        if (items[i] && items[i]->canUse(owner)) {
            BaseItem* ret = items[i];
            items[i] = nullptr;
            --count;
            return ret;
        }
    }
    return nullptr;
}

BaseItem* BaseBag::get(ItemType itemType) {
    if (!owner) return nullptr;

    for (int i = 0; i < capacity; ++i) {
        if (items[i] && items[i]->getType() == itemType && items[i]->canUse(owner)) {
            BaseItem* item = items[i];
            items[i] = nullptr;
            --count;
            return item;
        }
    }
    return nullptr;
}

string BaseBag::str() const {
    string result = "Bag[count=" + to_string(count) + ";";
    for (int i = 0; i < capacity; ++i) {
        if (items[i] != nullptr) {
            result += items[i]->str();
            if (i < capacity - 1) {
                result += ", ";
            }
        }
    }
    result += "]";
    return result;
}

void BaseBag::setOwner(Warrior* w) {
    owner = w;
}

//TeamBag
TeamBag::TeamBag(Warrior* owner) : BaseBag(10), bag_count(0) {
    this->owner = owner; 
    arrBaseBag = new BaseBag*[capacity];
    for (int i = 0; i < capacity; ++i) arrBaseBag[i] = nullptr;
}

TeamBag::~TeamBag() {
    for (int i = 0; i < bag_count; ++i) {
        delete arrBaseBag[i];
    }
    delete[] arrBaseBag;
}

bool TeamBag::insert(BaseBag* item) {
    if (!item || bag_count >= capacity) return false;
    arrBaseBag[bag_count++] = item;
    return true;
}

BaseBag* TeamBag::get(int index) {
    if (index < 0 || index >= bag_count) return nullptr;
    return arrBaseBag[index];
}

// DragonWarriorsProgram
DragonWarriorsProgram::DragonWarriorsProgram(const string &config_file_path) {
    config = new Configuration(config_file_path);
    
    map = nullptr;
    flyteam1 = nullptr;
    flyteam2 = nullptr;
    groundteam = nullptr;
    dragonlord = nullptr;
    arr_mv_objs = nullptr;
}


DragonWarriorsProgram::~DragonWarriorsProgram() {
    delete map;
    delete flyteam1;
    delete flyteam2;
    delete groundteam;
    delete dragonlord;
    delete arr_mv_objs;
    delete config;
}



////////////////////////////////////////////////
/// END OF STUDENT'S ANSWER
////////////////////////////////////////////////