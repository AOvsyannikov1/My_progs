import random

N = 3
GAME = ['•', '•', '•', '•', '•', '•', '•', '•', '•']


def print_pole():
    for i in range(N):
        print(GAME[3 * i], GAME[3 * i + 1], GAME[3 * i + 2])


def game_continues():
    for i in range(N):
        if GAME[i] == GAME[i + 3] == GAME[i + 6] == 'X':
            print("Крестики выиграли")
            return False
        if GAME[i] == GAME[i + 3] == GAME[i + 6] == '0':
            print("Нолики выиграли")
            return False
    for i in (0, 3, 6):
        if GAME[i] == GAME[i + 1] == GAME[i + 2] == 'X':
            print("Крестики выиграли")
            return False
        if GAME[i] == GAME[i + 1] == GAME[i + 2] == '0':
            print("Нолики выиграли")
            return False
    if GAME[0] == GAME[4] == GAME[8] == 'X':
        print("Крестики выиграли")
        return False
    if GAME[0] == GAME[4] == GAME[8] == '0':
        print("Нолики выиграли")
        return False
    if GAME[2] == GAME[4] == GAME[6] == 'X':
        print("Крестики выиграли")
        return False
    if GAME[2] == GAME[4] == GAME[6] == '0':
        print("Нолики выиграли")
        return False

    dots = 0
    for i in range(9):
        if GAME[i] == '•': dots += 1
    if dots > 0:
        return True
    else:
        return False


def go_player():
    if game_continues():
        input_cycle = True

        while input_cycle:
            x, y = input("Введите координаты: ").split()
            if not x.isdigit() or not y.isdigit():
                print("Неверные координаты")
                continue
            else:
                x = int(x) - 1
                y = int(y) - 1
                if x > 2 or y > 2 or x < 0 or y < 0 or GAME[x * N + y] == '0':
                    print("Неверные координаты")
                else:
                    input_cycle = False
        return x, y


def go_computer():
    rng = random.Random()
    comp_cycle = True
    while comp_cycle:
        x = rng.randrange(N)
        y = rng.randrange(N)
        if GAME[x * N + y] == '•':
            comp_cycle = False
    return x, y


def game_process():
    print_pole()
    while game_continues():
        x, y = go_player()
        GAME[x * N + y] = 'X'
        if game_continues():
            x, y = go_computer()
            GAME[x * N + y] = '0'
        print_pole()


game_process()

