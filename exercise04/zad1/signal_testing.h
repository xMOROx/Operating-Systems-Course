typedef enum Mode { IGNORE, MASK, HANDLER, PENDING } Mode;
typedef enum Action { FORK, EXEC } Action;

static char mode_str[4][10] = {"ignore", "mask", "handler", "pending"};

int parse_first_argument(char *arg, Mode *mode);
int parse_second_argument(char *arg, Action *action);
int parse_arguments(char *arg[], Mode *mode, Action *action);
int parse_mode(Mode mode);
int parse_action(Action action);