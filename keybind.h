static const struct {
	const char *key;
	const char *command;
} keys[] = {
	{ "a", "add-hydrogens" },
	{ "S-a", "select-element H" },
	{ "b", "bond" },
	{ "c", "view-center-selection" },
	{ "S-c", "view-fit-selection" },
	{ "d", "delete" },
	{ "Delete", "delete" },
	{ "e", "unselect *" },
	{ "f", "view-zoom 1.15" },
	{ "g", "view-zoom 0.87" },
	{ "S-f", "view-rotate 0 0 -10" },
	{ "S-g", "view-rotate 0 0 10" },
	{ "h", "view-rotate 0 10 0" },
	{ "j", "view-rotate 10 0 0" },
	{ "k", "view-rotate -10 0 0" },
	{ "l", "view-rotate 0 -10 0" },
	{ "S-h", "view-move -0.1 0 0" },
	{ "S-j", "view-move 0 0.1 0" },
	{ "S-k", "view-move 0 -0.1 0" },
	{ "S-l", "view-move 0.1 0 0" },
	{ "i", "toggle id-visible" },
	{ "S-i", "toggle name-visible" },
	{ "m", "measure" },
	{ "n", "select-within" },
	{ "o", "toggle origin-visible" },
	{ "S-o", "move-selection-to" },
	{ "p", "paste" },
	{ "C-v", "paste" },
	{ "q", "quit" },
	{ "S-q", "quit!" },
	{ "r", "record" },
	{ "s", "select" },
	{ "S-s", "select-connected" },
	{ "t", "replay" },
	{ "u", "unselect" },
	{ "v", "toggle-atoms" },
	{ "w", "select-molecule" },
	{ "S-w", "select-water" },
	{ "x", "copy; delete" },
	{ "C-x", "copy; delete" },
	{ "y", "copy" },
	{ "C-c", "copy" },
	{ "C-a", "select *" },
	{ "C-f", "rotate-selection 0 0 -10" },
	{ "C-g", "rotate-selection 0 0 10" },
	{ "C-h", "rotate-selection 0 10 0" },
	{ "C-j", "rotate-selection 10 0 0" },
	{ "C-k", "rotate-selection -10 0 0" },
	{ "C-l", "rotate-selection 0 -10 0" },
	{ "CS-h", "move-selection -0.1 0 0" },
	{ "CS-j", "move-selection 0 0.1 0" },
	{ "CS-k", "move-selection 0 -0.1 0" },
	{ "CS-l", "move-selection 0.1 0 0" },
	{ "C-r", "redo" },
	{ "C-z", "undo" },
	{ "Left", "view-rotate 0 10 0" },
	{ "Down", "view-rotate 10 0 0" },
	{ "Up", "view-rotate -10 0 0" },
	{ "Right", "view-rotate 0 -10 0" },
	{ "S-Left", "view-move -0.1 0 0" },
	{ "S-Down", "view-move 0 0.1 0" },
	{ "S-Up", "view-move 0 -0.1 0" },
	{ "S-Right", "view-move 0.1 0 0" },
	{ "C-Left", "rotate-selection 0 10 0" },
	{ "C-Down", "rotate-selection 10 0 0" },
	{ "C-Up", "rotate-selection -10 0 0" },
	{ "C-Right", "rotate-selection 0 -10 0" },
	{ "CS-Left", "move-selection -0.1 0 0" },
	{ "CS-Down", "move-selection 0 0.1 0" },
	{ "CS-Up", "move-selection 0 -0.1 0" },
	{ "CS-Right", "move-selection 0.1 0 0" },
	{ "=", "invert-selection" },
	{ "Space", "fullscreen" },
	{ "`", "view-reset" },
	{ "[", "next-frame -1" },
	{ "]", "next-frame 1" },
	{ "S-[", "next-frame -100" },
	{ "S-]", "next-frame 100" },
	{ "S-,", "prev-tab" },
	{ "S-.", "next-tab" },
};
