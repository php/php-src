/* 
    Temporary implementation of 'env' stuff till they are properly available in LibC
*/

#include <stdlib.h>
#include <nks/netware.h>

#include "env.h"

/*
#define MAX_VARIABLE_LEN    255
#define MAX_VALUE_LEN       1024
*/

/*int scanenv(int *sequence, char *variable, size_t *length, char *value );*/   /* From CLib */

char **environ = NULL;

/*
char *getEnvStr(char *name, char *defaultValue)
{
    char* ret = getenv(name);

    if (ret == NULL)
        ret = defaultValue;

    return ret;
}

char ***getEnvBlock()
{
    if (!environ)
        setupEnvBlock (&environ);

	return (&environ);
}
*/

/*
void setupEnvBlock(char*** pEnv)
{
    char** env = NULL;
    int sequence = 0;
    char var[MAX_VARIABLE_LEN+1] = {'\0'};
    char val[MAX_VALUE_LEN+1] = {'\0'};
    char both[MAX_VARIABLE_LEN + MAX_VALUE_LEN + 5] = {'\0'};
    size_t len  = MAX_VALUE_LEN;
    int totalCount = 0, count = 0, i = 0;

    while(scanenv(&sequence, var, &len, val))
    {
        totalCount++;
        len  = MAX_VALUE_LEN;
    }
    totalCount++;*/   /* add one for null termination
*/
/*
    env = (char **)malloc(totalCount * sizeof(char*));
    if (env)
    {
        sequence = 0;
        len  = MAX_VALUE_LEN;

        while ((count < (totalCount - 1)) && scanenv(&sequence, var, &len, val))
        {
            val[len] = '\0';
            strcpy (both, var);
            strcat (both, (char *)"=");
            strcat (both, val);

            env[count] = (char *)malloc((sizeof(both) + 1) * sizeof(char));
            if (env[count])
            {
                strcpy(env[count], both);
                count++;
            }
            else
            {
                for (i = 0; i < count; i++)
                {
                    if (env[i])
                    {
                        free (env[i]);
                        env[i] = NULL;
                    }
                }

                free (env);
                env = NULL;

                return;
            }

            len  = MAX_VALUE_LEN;
        }

        for (i = count; i <= (totalCount - 1); i++)
            env[i] = NULL;
    }
    else
        return;

    *pEnv = env;

    return;
}

void destroyEnvBlock(char** env)
{
    int k = 0;

    if (!env)
        return;

    while (env[k] != NULL)
    {
        free (env[k]);
        env[k] = NULL;
        k++;
    }

    free (env);
	env = NULL;

    return;
}
*/

/* Added to avoid 'scanenv' problem */
void getEnvironPtr()
{
    environ = nxGetEnviron();
}

