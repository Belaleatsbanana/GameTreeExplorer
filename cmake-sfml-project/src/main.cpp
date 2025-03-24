#include "objects/GameManager.h"

int main()
{
    GameManager<5, 5, 3> gameManager;
    gameManager.run();
    return 0;
}