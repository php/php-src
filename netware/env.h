/* 
    Temporary implementation of 'env' stuff till they are properly available in LibC
*/


/*
    Global variable to hold the environ information.
    First time it is accessed, it will be created and initialized and 
    next time onwards, the pointer will be returned.
    Improvements: dynamically read env everytime a request comes -- is this required?
*/

/*
    So that the original 'environ' isn't used -- this is temporary
    and will go when this file goes
*/
#undef environ

/*
void setupEnvBlock(char*** pEnv);
void destroyEnvBlock(char** env);
*/
void getEnvironPtr();	/* New function to get 'environ' pointer */
