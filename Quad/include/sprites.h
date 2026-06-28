// clang-format off
const char* HappyFace[8] = {
    "    ^     ^    ",
    "   ^ ^   ^ ^   ",
    "   ###   ###   ",
    "   ###   ###   ", 
    "       @       ", 
    "   #       #   ",
    "    #######    ", 
    "               "};

const char* LookLeft[8] = {
    "               ", 
    "               ", 
    " ###   ###     ",
    " ###   ###     ", 
    " ###   ###     ", 
    " ###   ###     ",
    "               ", 
    "               "};

const char* LookRight[8] = {
    "               ", 
    "               ", 
    "      ###  ### ",
    "      ###  ### ", 
    "      ###  ### ", 
    "      ###  ### ",
    "               ", 
    "               "};

// Possible combinations:
// - Left + Left
const char* LookLeftLeft[8] = {
    "               ", 
    "               ", 
    " ###  ###      ",
    " ###  ###      ", 
    " ###  ###      ", 
    " ###  ###      ",
    "               ", 
    "               "};

// - Left + Right
const char* LookLeftRight[8] = {
    "               ", 
    "               ", 
    " ###       ### ",
    " ###       ### ", 
    " ###       ### ", 
    " ###       ### ",
    "               ", 
    "               "};
// - Left + Up
const char* LookLeftUp[8] = {
    "         ###   ", 
    "         ###   ", 
    " ###     ###   ",
    " ###     ###   ", 
    " ###           ", 
    " ###           ",
    "               ", 
    "               "};
// - Left + Down
const char* LookLeftDown[8] = {
    "               ", 
    "               ", 
    " ###           ",
    " ###           ", 
    " ###     ###   ", 
    " ###     ###   ",
    "         ###   ", 
    "         ###   "};
// - Right + Left
const char* LookRightLeft[8] = {
    "               ", 
    "               ", 
    "    ### ###    ",
    "    ### ###    ", 
    "    ### ###    ", 
    "    ### ###    ",
    "               ", 
    "               "};
// - Right + Right
const char* LookRightRight[8] = {
    "               ", 
    "               ", 
    "      ###  ### ",
    "      ###  ### ", 
    "      ###  ### ", 
    "      ###  ### ",
    "               ", 
    "               "};
// - Right + Up
const char* LookRightUp[8] = {
    "         ###   ", 
    "         ###   ", 
    "     ### ###   ",
    "     ### ###   ", 
    "     ###       ", 
    "     ###       ",
    "               ", 
    "               "};
// - Right + Down
const char* LookRightDown[8] = {
    "               ", 
    "               ", 
    "     ###       ",
    "     ###       ", 
    "     ### ###   ", 
    "     ### ###   ",
    "         ###   ", 
    "         ###   "};
// - Up + Left
const char* LookUpLeft[8] = {
    "   ###         ", 
    "   ###         ", 
    "   ### ###     ",
    "   ### ###     ", 
    "       ###     ", 
    "       ###     ",
    "               ", 
    "               "};
// - Up + Right
const char* LookUpRight[8] = {
    "   ###         ", 
    "   ###         ", 
    "   ###     ### ",
    "   ###     ### ", 
    "           ### ", 
    "           ### ",
    "               ", 
    "               "};
// - Up + Up
const char* LookUpUp[8] = {
    "   ###   ###   ", 
    "   ###   ###   ", 
    "   ###   ###   ",
    "   ###   ###   ", 
    "               ", 
    "               ",
    "               ", 
    "               "};
// - Up + Down
const char* LookUpDown[8] = {
    "   ###         ", 
    "   ###         ", 
    "   ###         ",
    "   ###         ", 
    "         ###   ", 
    "         ###   ",
    "         ###   ", 
    "         ###   "};
// - Down + Left
const char* LookDownLeft[8] = {
    "               ", 
    "       ###     ", 
    "       ###     ",
    "       ###     ", 
    "   ### ###     ", 
    "   ###         ",
    "   ###         ", 
    "   ###         "};
// - Down + Right
const char* LookDownRight[8] = {
    "               ", 
    "               ", 
    "           ### ",
    "           ### ", 
    "    ###    ### ", 
    "    ###    ### ",
    "    ###        ", 
    "    ###        "};
// - Down + Up
const char* LookDownUp[8] = {
    "         ###   ", 
    "         ###   ", 
    "         ###   ",
    "         ###   ", 
    "    ###        ", 
    "    ###        ",
    "    ###        ", 
    "    ###        "};
// - Down + Down
const char* LookDownDown[8] = {
    "               ", 
    "               ", 
    "               ",
    "               ", 
    "   ###   ###   ", 
    "   ###   ###   ",
    "   ###   ###   ", 
    "   ###   ###   "};
// clang-format on