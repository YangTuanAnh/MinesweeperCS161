#include <raylib.h>
#include <raygui.h>
#define RAYGUI_IMPLEMENTATION

#include "minesweeper.h"

using namespace std;

bool pause = false;

const Color PRIMARY = {189, 74, 74, 255}, SECONDARY = {252, 105, 85, 255}, TRITARY = {229, 155, 95, 255};

int main()
{ //
    // Tạo window
    InitWindow(WIDTH_WINDOW, HEIGHT_WINDOW, "Minesweeper by Yang Tuấn Anh & Từ Cảnh Minh");
    // Tạo window icon
    windowicon();
    // Đặt FPS cho game loop
    SetTargetFPS(60);

    // OST
    InitAudioDevice();
    Music music = LoadMusicStream("..\\music\\song.mp3");
    PlayMusicStream(music);

    // GUI Style cho RayGui
    GuiLoadStyle("..\\include\\candy.rgs");

    Program currentProgram;
    User currentUser;
    currentUser.load_input();

    while (!WindowShouldClose())
    {
        UpdateMusicStream(music);
        if (IsKeyPressed(KEY_P))
        {
            pause = !pause;

            if (pause)
                PauseMusicStream(music);
            else
                ResumeMusicStream(music);
        }
        switch (currentProgram.screen)
        {
        case LOGO:
        {
            // Đợi 2s rồi chuyển qua title
            currentProgram.framesCounter++;
            if (currentProgram.framesCounter > 120)
                currentProgram.screen = TITLE;
        }
        break;
        case TITLE:
        {
            // Bấm New Game để qua DIFFICULTY
            if (GuiButton(Rectangle({250, 250, WIDTH_WINDOW - 250 * 2, 30}), "New Game"))
                currentProgram.screen = DIFFICULTY;
            // Bấm Continue để qua GAMEPLAY nếu chưa cần tạo mới game
            if (GuiButton(Rectangle({250, 290, WIDTH_WINDOW - 250 * 2, 30}), "Continue") && !currentUser.tab.isActive)
                currentProgram.screen = GAMEPLAY;
            // Bấm Settings để qua SETTINGS
            if (GuiButton(Rectangle({250, 330, WIDTH_WINDOW - 250 * 2, 30}), "Settings"))
                currentProgram.screen = SETTINGS;
            // Bấm Save & Exit Game để load dữ liệu vào file .dat và đóng window
            if (GuiButton(Rectangle({250, 370, WIDTH_WINDOW - 250 * 2, 30}), "Save & Exit Game"))
            {
                currentUser.save_output();
                CloseWindow();
            }
        }
        break;
        case DIFFICULTY:
        {
            // Bấm EASY để tạo wbomb=8, hbomb=8, vbomb=10, tạo mới game, chuyển qua GAMEPLAY
            if (GuiButton(Rectangle({250, 250, WIDTH_WINDOW - 250 * 2, 30}), "EASY"))
            {
                currentUser.create(8, 8, 10);
                currentProgram.screen = GAMEPLAY;
            }
            // Bấm INTERMEDIATE để tạo wbomb=16, hbomb=12, vbomb=50, tạo mới game, chuyển qua GAMEPLAY
            if (GuiButton(Rectangle({250, 290, WIDTH_WINDOW - 250 * 2, 30}), "INTERMEDIATE"))
            {
                currentUser.create(16, 12, 50);
                currentProgram.screen = GAMEPLAY;
            }
            // Bấm HARD để tạo wbomb=24, hbomb=16, vbomb=100, tạo mới game, chuyển qua GAMEPLAY
            if (GuiButton(Rectangle({250, 330, WIDTH_WINDOW - 250 * 2, 30}), "HARD"))
            {
                currentUser.create(24, 16, 100);
                currentProgram.screen = GAMEPLAY;
            }
            // Bấm CUSTOM để chuyển qua CUSTOM
            if (GuiButton(Rectangle({250, 370, WIDTH_WINDOW - 250 * 2, 30}), "CUSTOM"))
                currentProgram.screen = CUSTOM;
            // Bấm BACK để chuyển qua BACK
            // the user's current gameplay should be noticed ...
            if (GuiButton(Rectangle({250, 410, WIDTH_WINDOW - 250 * 2, 30}), "BACK"))
                currentProgram.screen = TITLE;
        }
        break;
        case CUSTOM:
        {
            // Bấm CREATE để chuyển qua GAMEPLAY
            if (GuiButton(Rectangle({250, 250, WIDTH_WINDOW - 250 * 2, 30}), "CREATE"))
            {
                currentUser.create(currentUser.tab.wbomb, currentUser.tab.hbomb, currentUser.tab.vbomb);
                currentProgram.screen = GAMEPLAY;
            }
            // 3 thanh slider để chỉnh giá trị wbomb, hbomb, vbomb
            GuiSpinner(Rectangle({250, 310, WIDTH_WINDOW - 250 * 2, 20}), "WIDTH_WINDOW",
                       &currentUser.tab.wbomb, MIN_WIDTH_TABLE, MAX_WIDTH_TABLE, false);
            GuiSpinner(Rectangle({250, 340, WIDTH_WINDOW - 250 * 2, 20}), "HEIGHT_WINDOW",
                       &currentUser.tab.hbomb, MIN_HEIGHT_TABLE, MAX_HEIGHT_TABLE, false);
            GuiSpinner(Rectangle({250, 370, WIDTH_WINDOW - 250 * 2, 20}), "bombs",
                       &currentUser.tab.vbomb, 2, (currentUser.tab.wbomb * currentUser.tab.hbomb) / 2, false);
        }
        break;
        case GAMEPLAY:
        {
            // Chạy timer
            currentUser.timerCount();
            if (currentUser.tab.isActive)
                currentUser.generate();
            // Lấy vị trí click chuột và toạ độ hoá trên ma trận grid
            Vector2 mousePos = GetMousePosition();
            currentProgram.mousex = (mousePos.x - (WIDTH_WINDOW - 32 * currentUser.tab.wbomb) / 2) / 32;
            currentProgram.mousey = (mousePos.y - (HEIGHT_WINDOW - 32 * currentUser.tab.hbomb) / 2) / 32 - 1;
            // Nếu click trái vào ma trận
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && currentUser.tab.isSafe(currentProgram.mousey, currentProgram.mousex) && !currentUser.tab.flagged[currentProgram.mousey][currentProgram.mousex] && !currentUser.tab.opened[currentProgram.mousey][currentProgram.mousex])
            {
                if (currentUser.tab.grid[currentProgram.mousey][currentProgram.mousex] == BOMB)
                {
                    currentProgram.screen = WINLOSE;
                    currentUser.win = false;
                    currentUser.score *= currentUser.timer;
                }
                else
                {
                    // Ko phải bomb thì score++
                    currentUser.score++;
                    currentUser.tab.opened[currentProgram.mousey][currentProgram.mousex] = true;
                    currentUser.tab.flagged[currentProgram.mousey][currentProgram.mousex] = false;
                    // Nếu là ô trống thi bfs các ô trống xung quanh
                    if (currentUser.tab.grid[currentProgram.mousey][currentProgram.mousex] == 0)
                    {
                        currentUser.BFS(currentProgram.mousey, currentProgram.mousex);
                    }
                }
            }
            // Nếu click chuột phải
            else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && currentUser.tab.isSafe(currentProgram.mousey, currentProgram.mousex) && !currentUser.tab.opened[currentProgram.mousey][currentProgram.mousex])
            {
                // Đổi giữa đặt cờ và ko đặt cờ
                cerr << "HERE " << __LINE__ << endl;
                currentUser.tab.flagged[currentProgram.mousey][currentProgram.mousex] = !currentUser.tab.flagged[currentProgram.mousey][currentProgram.mousex];
            }
            // Bấm BACK để chuyển qua TITLE
            if (GuiButton(Rectangle({740, 10, 50, 30}), "BACK"))
                currentProgram.screen = TITLE;
            // Bấm CHECK để dò số bomb đã đặt cờ, nếu đủ thì end game và win
            if (GuiButton(Rectangle({740, 50, 50, 30}), "CHECK"))
            {
                int cnt = 0;
                for (int i = 0; i < currentUser.tab.hbomb; i++)
                    for (int j = 0; j < currentUser.tab.wbomb; j++)
                        if (currentUser.tab.grid[i][j] == BOMB && currentUser.tab.flagged[i][j])
                            cnt++;
                if (cnt == currentUser.tab.vbomb)
                {
                    currentProgram.screen = WINLOSE;
                    currentUser.win = true;
                    currentUser.score *= currentUser.timer;
                }
            }
        }
        break;
        case WINLOSE:
        {
            // Bấm BACK để chuyển qua TITLE, cho phép tạo mới game
            if (GuiButton(Rectangle({250, 280, WIDTH_WINDOW - 250 * 2, 30}), "BACK"))
            {
                currentProgram.screen = TITLE;
                currentUser.tab.isActive = true;
            }
        }
        break;
        case SETTINGS:
        {
            // Đặt hiscore
            currentUser.hiscore = max(currentUser.score, currentUser.hiscore);
            // Reset Data, cho phép tạo mới game
            if (GuiButton(Rectangle({250, 430, WIDTH_WINDOW - 250 * 2, 30}), "RESET DATA"))
            {
                currentUser.tab.wbomb = currentUser.tab.hbomb = currentUser.tab.vbomb =
                    currentUser.timer = currentUser.timerCounter = currentUser.score = currentUser.hiscore = 0;
                currentUser.tab.isActive = true;
            }
            // Bấm BACK để chuyển qua TITLE
            if (GuiButton(Rectangle({250, 470, WIDTH_WINDOW - 250 * 2, 30}), "BACK"))
                currentProgram.screen = TITLE;
        }
        break;
        default:
            break;
        }

        // - - - DRAWING - - - //
        BeginDrawing();

        ClearBackground({255, 245, 225, 225});
        DrawCircle(40, 20, 50, {189, 74, 74, 255 / 2});
        DrawCircle(20, 40, 100, {252, 105, 85, 255 / 2});
        DrawCircle(WIDTH_WINDOW - 40, HEIGHT_WINDOW - 20, 50, {189, 74, 74, 255 / 2});
        DrawCircle(WIDTH_WINDOW - 20, HEIGHT_WINDOW - 40, 100, {252, 105, 85, 255 / 2});
        switch (currentProgram.screen)
        {
        case LOGO:
        {
            DrawText("LOGO SCREEN", 20, 20, 40, PRIMARY);
            DrawText("WAIT for 2 SECONDS...", 290, 220, 20, PRIMARY);
        }
        break;
        case TITLE:
        {
            DrawText("MINESWEEPER", 20, 20, 40, PRIMARY);
            DrawText("CS161 Finals Project by Yang Tuan Anh & Tu Canh Minh", 120, 220, 20, PRIMARY);
        }
        break;
        case DIFFICULTY:
        {
            DrawText("DIFFICULTY SCREEN", 20, 20, 40, PRIMARY);
            DrawText("PRESS EASY, INTERMEDIATE or HARD to GAMEPLAY SCREEN", 100, 220, 20, PRIMARY);
        }
        break;
        case CUSTOM:
        {
            DrawText("CUSTOM SCREEN", 20, 20, 40, PRIMARY);
            DrawText("PRESS CREATE to GAMEPLAY SCREEN", 200, 220, 20, PRIMARY);
        }
        break;
        case GAMEPLAY:
        {
            DrawText("GAMEPLAY SCREEN", 20, 20, 40, PRIMARY);

            // In các thông số score, timer, vbomb
            DrawText(TextFormat("SCORE: %i", currentUser.score), 450, 30, 16, TRITARY);
            DrawText(TextFormat("TIMER: %i", currentUser.timer), 550, 30, 16, TRITARY);
            DrawText(TextFormat("BOMBS: %i", currentUser.tab.vbomb), 650, 30, 16, TRITARY);

            // In các ô ma trận dựa theo sgrid
            for (int i = 0; i < currentUser.tab.hbomb; i++)
            {
                for (int j = 0; j < currentUser.tab.wbomb; j++)
                {
                    float posx = (WIDTH_WINDOW - 32 * currentUser.tab.wbomb) / 2 + 32 * j;
                    float posy = 72 + (HEIGHT_WINDOW - 72 - 32 * currentUser.tab.hbomb) / 2 + 32 * i;

                    // I don't know how to interpret this line, please fix
                    // sgrid[i][j] >= 10 ? DrawRectangle(posx, posy, 32, 32, PRIMARY) : DrawRectangle(posx, posy, 32, 32, SECONDARY);
                    if (currentUser.tab.opened[i][j])
                        DrawRectangle(posx, posy, 32, 32, SECONDARY);
                    else
                        DrawRectangle(posx, posy, 32, 32, PRIMARY);

                    if (currentUser.tab.flagged[i][j])
                    {
                        // a flagged cell is supposed to be not opened yet
                        assert(!currentUser.tab.opened[i][j]);
                        DrawText("?", posx + 8, posy + 8, 16, RAYWHITE);
                    }
                    if (currentUser.tab.opened[i][j])
                    {
                        if (currentUser.tab.grid[i][j] == BOMB)
                            DrawText("*", posx + 8, posy + 8, 16, RAYWHITE);
                        else if (0 < currentUser.tab.grid[i][j] && currentUser.tab.grid[i][j] <= 8)
                            DrawText(TextFormat("%d", currentUser.tab.grid[i][j]), posx + 8, posy + 8, 16, RAYWHITE);
                    }

                    DrawRectangleLines(posx, posy, 32, 32, SECONDARY);
                }
            }
        }
        break;
        case WINLOSE:
        {
            // In Score, Hiscore, Win/Lose
            DrawText("WINLOSE SCREEN", 20, 20, 40, PRIMARY);
            DrawText(TextFormat("SCORE: %i", currentUser.score), 20, 60, 20, TRITARY);
            DrawText(TextFormat("HISCORE: %i", currentUser.hiscore), 20, 90, 20, TRITARY);
            DrawText((currentUser.win ? "WIN!" : "LOSE"), 380, 220, 20, SECONDARY);
            DrawText("PRESS BACK to TITLE SCREEN", 230, 250, 20, PRIMARY);
        }
        break;
        case SETTINGS:
        {
            // In các thông số Hiscore, score, timer, wbomb, hbomb, vbomb
            DrawText("SETTINGS SCREEN", 20, 20, 40, PRIMARY);
            DrawText("PRESS P to PLAY/PAUSE MUSIC", 220, 190, 20, PRIMARY);
            DrawText("PRESS BACK to TITLE SCREEN", 230, 220, 20, PRIMARY);
            DrawText(TextFormat("HISCORE: %i", currentUser.hiscore, "%c;"), 350, 250, 20, TRITARY);
            DrawText(TextFormat("SCORE: %i", currentUser.score), 350, 280, 20, TRITARY);
            DrawText(TextFormat("TIMER: %i", currentUser.timer), 350, 310, 20, TRITARY);
            DrawText(TextFormat("BOMBS: %i", currentUser.tab.vbomb), 350, 340, 20, TRITARY);
            DrawText(TextFormat("WIDTH_WINDOW: %i", currentUser.tab.wbomb), 350, 370, 20, TRITARY);
            DrawText(TextFormat("HEIGHT_WINDOW: %i", currentUser.tab.hbomb), 350, 400, 20, TRITARY);
        }
        break;
        default:
            break;
        }

        EndDrawing();
    }
    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}