#include "sigHelper.h"

int main(int argc, char *argv[]) {
    if (argc != 2) raiseError("You must pass exactly one argument.");
    puts("\nEXEC:");

    Mode mode = getMode(argv[1]);
    raise(SIGUSR1);
    switch (mode) {
        case IGNORE:
            puts("Ignore in exec:");
            break;
            
        case HANDLER:
        	break;

        case MASK:
            puts("Mask in exec:");
            checkMask();
            break;

        case PENDING:
            puts("Pending status in exec:");
            checkPending();
            break;
    }
	
	puts("END OF EXEC");
    return 0;
}
