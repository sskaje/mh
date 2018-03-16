#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

int main()
{
    char* input, shell_prompt[100];

    // Configure readline to auto-complete paths when the tab key is hit.
    rl_bind_key('\t', rl_complete);

    for(;;) {
        // Create prompt string from user name and current working directory.
        snprintf(shell_prompt, sizeof(shell_prompt), "%s:%s $ ", getenv("USER"), getcwd(NULL, 1024));

        // Display prompt and read input (NB: input must be freed after use)...
        input = readline(shell_prompt);

        // Check for EOF.
        if (!input)
            break;

        // Add input to history.
        add_history(input);

        // Do stuff...

        // Free input.
        free(input);
    }
    return 0;
}