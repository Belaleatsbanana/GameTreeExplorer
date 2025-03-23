#include "objects/GameManager.h"

int main()
{
    GameManager<10, 10, 8> gameManager;
    gameManager.run();
    return 0;
}