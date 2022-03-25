#include <stdio.h>
#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

int QUESTION_GAP = 140;
int FONT_SIZE = 28;

bool RETURN_EXIT = false;
int BASE_X_POS = 10;
int BASE_Y_POS = 10;
int BASE_X_ANSWER_POS = 20;
Uint32 BUTTON_CLICKED_EVENT = -1;
int LEFT_MOUSE_BUTTON_DRAG_POS[2] = {0, 0};
int RIGHT_MOUSE_BUTTON_DRAG_POS[2] = {0, 0};
int Category = 0;

bool NeedWindowRedraw = false;

SDL_Window *Window = NULL;
SDL_Renderer *Renderer = NULL;
TTF_Font *Font = NULL;

int TotalPoint = 0;
bool PointUpdate = true;

int multi210[9] = {2, 3, 4, 6, 7, 14, 16, 17, 18};

struct ButtonData
{
    int x_pos = 0;
    int y_pos = 0;
    int width = 0;
    int height = 0;
    std::string id;
    bool picked = false;
};

std::vector<ButtonData> ButtonVector;

bool addButton(int x_pos, int y_pos, int width, int height, std::string id)
{
    ButtonVector.push_back(ButtonData());
    int index = ButtonVector.size() - 1;
    ButtonVector[index].x_pos = x_pos;
    ButtonVector[index].y_pos = y_pos;
    ButtonVector[index].width = width;
    ButtonVector[index].height = height;
    ButtonVector[index].id = id;
    return true;
}

struct QuestionData
{
    std::string question;
    std::string answer_a;
    std::string answer_b;
    std::string answer_c;
    bool no_button = false;
};

std::vector<QuestionData> QuestionVector;

bool addQuestion(std::string question, std::string answer_a, std::string answer_b, std::string answer_c, bool no_button = false)
{
    QuestionVector.push_back(QuestionData());
    int index = QuestionVector.size() - 1;
    QuestionVector[index].question = question.c_str();
    QuestionVector[index].answer_a = answer_a.c_str();
    QuestionVector[index].answer_b = answer_b.c_str();
    QuestionVector[index].answer_c = answer_c.c_str();
    QuestionVector[index].no_button = no_button;
    return true;
}

struct TextureData
{
    SDL_Texture *texture = NULL;
    int width = 0;
    int height = 0;
};

TextureData TotalPointT;
TextureData TotalPointS;
TextureData PickedButtonT;
TextureData CategoryT;

// UNUSED
// std::vector<TextureData> TextureVector;

// bool addTexture(SDL_Texture *texture, int width, int height)
// {
//     TextureVector.push_back(TextureData());
//     int index = TextureVector.size() - 1;
//     TextureVector[index].texture = texture;
//     TextureVector[index].width = width;
//     TextureVector[index].height = height;
//     return true;
// }

bool renderTexture(SDL_Texture *texture, int x_pos, int y_pos, int width, int height)
{
    SDL_Rect quad = {x_pos, y_pos, width, height};
    return SDL_RenderCopy(Renderer, texture, NULL, &quad) >= 0;
}

TextureData createTexture(std::string text, int color)
{
    TextureData Texture;
    SDL_Color C = {0, 0, 0};
    if (color == 1)
        C = {0x00, 0xff, 0x40};
    if (color == 2)
        C = {0xdc, 0x14, 0x3c};
    SDL_Surface *Surf = TTF_RenderText_Solid(Font, text.c_str(), C);
    if (Surf == NULL)
    {
        return Texture;
    }

    if ((Texture.texture = SDL_CreateTextureFromSurface(Renderer, Surf)) == NULL)
    {
        Texture.texture = NULL;
        return Texture;
    }

    Texture.width = Surf->w;
    Texture.height = Surf->h;

    SDL_FreeSurface(Surf);
    return Texture;
}

struct QuestionTextureData
{
    TextureData questionT;
    TextureData answer_aT;
    TextureData answer_bT;
    TextureData answer_cT;
    int x_pos = 0;
    int y_pos = 0;
};

std::vector<QuestionTextureData> QuestionTextureVector;

QuestionTextureData createQuestionTexture(QuestionData data)
{
    QuestionTextureData Texture;
    Texture.questionT = createTexture(data.question, 0);
    Texture.answer_aT = createTexture(data.answer_a, 0);
    Texture.answer_bT = createTexture(data.answer_b, 0);
    Texture.answer_cT = createTexture(data.answer_c, 0);
    Texture.x_pos = 0;
    Texture.y_pos = QuestionTextureVector.size() * QUESTION_GAP;
    return Texture;
}

bool renderQuestionTexture(QuestionTextureData data)
{
    renderTexture(data.questionT.texture, BASE_X_POS + data.x_pos, BASE_Y_POS + data.y_pos, data.questionT.width, data.questionT.height);
    renderTexture(data.answer_aT.texture, BASE_X_POS + BASE_X_ANSWER_POS + data.x_pos + FONT_SIZE, BASE_Y_POS + data.y_pos + 30, data.answer_aT.width, data.answer_aT.height);
    renderTexture(data.answer_bT.texture, BASE_X_POS + BASE_X_ANSWER_POS + data.x_pos + FONT_SIZE, BASE_Y_POS + data.y_pos + 60, data.answer_bT.width, data.answer_bT.height);
    renderTexture(data.answer_cT.texture, BASE_X_POS + BASE_X_ANSWER_POS + data.x_pos + FONT_SIZE, BASE_Y_POS + data.y_pos + 90, data.answer_cT.width, data.answer_cT.height);
    return true;
}

bool destroyQuestionTexture(QuestionTextureData T)
{
    SDL_DestroyTexture(T.questionT.texture);
    T.questionT.texture = NULL;
    T.questionT.width = 0;
    T.questionT.height = 0;
    SDL_DestroyTexture(T.answer_aT.texture);
    T.answer_aT.texture = NULL;
    T.answer_aT.width = 0;
    T.answer_aT.height = 0;
    SDL_DestroyTexture(T.answer_bT.texture);
    T.answer_bT.texture = NULL;
    T.answer_bT.width = 0;
    T.answer_bT.height = 0;
    SDL_DestroyTexture(T.answer_cT.texture);
    T.answer_cT.texture = NULL;
    T.answer_cT.width = 0;
    T.answer_cT.height = 0;
    T.x_pos = 0;
    T.y_pos = 0;
    return true;
}

/**
 * @brief Initialize program
 *
 * @return int
 */
int Init_Program()
{
    if (TTF_Init() == -1)
    {
        return -1;
    }
    if (int ret = SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        return ret;
    }

    if ((Window = SDL_CreateWindow("FORM INSTRUMEN VERIFIKASI RUMAH TANGGA MISKIN", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL)) == NULL)
    {
        return -1;
    }

    if ((Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED)) == NULL)
    {
        return -1;
    }

    if (SDL_SetRenderDrawColor(Renderer, 0xFF, 0xFF, 0xFF, 0xFF) < 0)
    {
        return -1;
    };

    if ((BUTTON_CLICKED_EVENT = SDL_RegisterEvents(1)) == (Uint32)-1)
    {
        return -1;
    }

    if (!(Font = TTF_OpenFont("Times New Roman 400.ttf", FONT_SIZE)))
    {
        return -1;
    }

    addQuestion("1.a. Jumlah anggota rumah tangga yang bekerja (termasuk KK)", "1. Tidak ada", "2. 1-2 orang", "3. Lebih dari 2 orang");
    addQuestion("1.b. Pekerjaan Anggota Rumah Tangga (termasuk KK)", "1. Tidak bekerja", "2. Bekerja Tidak Tetap", "3. Bekerja Tetap");
    addQuestion("1.c. Total Pendapatan per kapita per bulan:", "1. Kurang dari Rp. 350.000", "2. Rp. 350.000 - Rp. 750.000", "3. Lebih dari Rp. 750.000");
    addQuestion("2.a. Tabungan / Barang yang mudah dijual:", "1. Tidak memiliki tabungan atau barang mudah dijual senilai kurang dari Rp. 500.000", "2. Rp. 500.000 - Rp. 2.500.000", "3. Lebih dari Rp. 2.500.000");
    addQuestion("2.b. Luas tanah yang dimiliki:", "1. Kurang dari 2.500 m2 (0.25 Ha)", "2. 2.500 - 5.000 m2", "3. Lebih dari 5000 m");
    addQuestion("3.a. Jumlah Anggota Rumah Tangga yang sakit", "1. Lebih dari 1 orang", "2. 1 orang", "3. Tidak ada");
    addQuestion("3.b. Kemampuan membayar pengobatan:", "1. Tidak mampu bayar biaya pengobatan (rawat jalan) di Puskesmas dan jaringannya", "2. Hanya mampu bayar biaya pengobatan (rawat jalan) di Puskesmas dan jaringannya", "3. Mampu bayar biaya pengobatan (rawat inap) di Puskesmas dan jaringannya");
    addQuestion("4.a. Sumber pangan sehari-hari:", "1. Tidak ada", "2. Bantuan dari pihak lain (anak, tetangga, saudara, dll.)", "3. Membeli dari pendapatannya sendiri");
    addQuestion("4.b. Persediaan pangan saat ini:", "1. Hanya tersedia untuk 1-2 hari", "2. Tersedia untuk 3-8 hari", "3. Tersedia untuk > 8 hari");
    addQuestion("4.c. Konsumsi Lauk Pauk saat ini:", "1. Tidak pernah mengkonsumsi daging/ikan laut/susu 1 (satu) kali seminggu", "2. Mengkonsumsi daging/ikan laut/susu 1 (satu) kali seminggu", "3. Mengkonsumsi daging/ikan laut/susu lebih dari 1 (satu) kali seminggu");
    addQuestion("5.a. Cara mendapatkan air bersih:", "1. Sumber air minum dari sumur milik tetangga/mata air tak terlindung/sungai/air hujan", "2. Sumber air minum dari sumur milik sendiri/mata air terlindung", "3. Sumber air minum dari jaringan air bersih");
    addQuestion("5.b. Fasilitas BAB:", "1. Tidak memiliki fasilitas buang air besar (BAB) atau bergabung dengan tetangga", "2. Memiliki fasilitas buang air besar (BAB) jenis cubluk/cemplung", "3. Memiliki fasilitas buang air besar (BAB) jenis leher angsa dengan septictank");
    addQuestion("6.a. Pendidikan Kepala Keluarga (Kepala Rumah Tangga)", "1. Tidak sekolah/tidak tamat/hanya tamat SD atau yang sederajat", "2. Tamat SMTP atau yang sederajat", "3. Minimal SMTA atau yang sederajat");
    addQuestion("6.b. Jumlah anggota rumah tangga yang masih bersekolah", "1. Lebih dari 1 orang", "2. 1 orang", "3. Tidak ada");
    addQuestion("7.a. Status tanah yang ditempati rumah:", "1. Magersari", "2. Warisan yang belum dibagi", "3. Milik sendiri");
    addQuestion("7.b. Kondisi Pondasi Rumah:", "1. Tidak berpondasi", "2. Pondasi dari batu bata", "3. Pondasi dari batu kali/gunung");
    addQuestion("7.c. Luas lantai rumah", "1. Luas lantai kurang dari 8 m2 per anggota keluarga", "2. Luas lantai 8 m2 per anggota keluarga", "3. Luas lantai > 8 m2 per anggota keluarga");
    addQuestion("7.d. Kondisi lantai rumah terluas:", "1. Lantai rumah/bangunan tempat tinggal dari tanah", "2. Plester/Rabat", "3. Keramik");
    addQuestion("7.e. Kondisi dinding rumah terluas:", "1. Dinding rumah dari bambu/kayu kualitas rendah", "2. Tembok tanpa plester/plester kualitas rendah", "3. Tembok plester kualitas baik");
    addQuestion("7.f. Kondisi atap terluas:", "1. Kerangka atap dari bambu", "2. Kerangka atap dari kayu berkualitas rendah", "3. Kerangka atap dari kayu kualitas baik, tidak tampias");
    addQuestion("8.a. Sumber Penerangan Rumah", "1. Penerangan rumah tidak menggunakan listrik", "2. Listrik menyalur dari tetangga", "3. Menggunakan listrik pasang sendiri");

    addQuestion("Kategori Rumah Tangga:", "a. Nilai Skor         :", "b. Kategori RTM  :", "", true);

    for (size_t i = 0; i < QuestionVector.size(); i++)
    {
        QuestionTextureData QT = createQuestionTexture(QuestionVector[i]);
        QuestionTextureVector.push_back(QT);
        std::string N = std::to_string(i);
        // printf("[N] %s\n", N.append(":somethin").c_str());
        if (QuestionVector[i].no_button)
            continue;
        addButton(BASE_X_POS + BASE_X_ANSWER_POS + QT.x_pos + FONT_SIZE, BASE_Y_POS + QT.y_pos + 30, FONT_SIZE * 2 / 3, FONT_SIZE, (N + ":0").c_str());
        addButton(BASE_X_POS + BASE_X_ANSWER_POS + QT.x_pos + FONT_SIZE, BASE_Y_POS + QT.y_pos + 60, FONT_SIZE * 2 / 3, FONT_SIZE, (N + ":1").c_str());
        addButton(BASE_X_POS + BASE_X_ANSWER_POS + QT.x_pos + FONT_SIZE, BASE_Y_POS + QT.y_pos + 90, FONT_SIZE * 2 / 3, FONT_SIZE, (N + ":2").c_str());
    }

    TotalPointS = createTexture("TOTAL POIN:", 0);
    PickedButtonT = createTexture("X", 1);

    NeedWindowRedraw = true;
    return 0;
}

void HandleEvent(SDL_Event *Event)
{
    // printf("[EVENT] %d\n", Event->type);
    if (Event->type == SDL_KEYUP)
    {
        if (Event->key.keysym.sym == SDLK_ESCAPE)
        {
            RETURN_EXIT = true;
        }
    }

    if (Event->type == SDL_WINDOWEVENT)
    {
        if (Event->window.event == SDL_WINDOWEVENT_EXPOSED)
        {
            NeedWindowRedraw = true;
        }
    }

    if (Event->type == SDL_MOUSEWHEEL)
    {
        // printf("[DIRECTION] %d\n", Event->wheel.y);
        if (Event->wheel.y > 0)
        {
            if (BASE_Y_POS < 10)
            {
                int add = BASE_Y_POS > -50 ? (-BASE_Y_POS + 10) : 50;
                for (size_t i = 0; i < ButtonVector.size(); i++)
                {
                    ButtonVector[i].y_pos += add;
                }
                BASE_Y_POS += add;
            }
        }
        else
        {
            BASE_Y_POS -= 50;
            for (size_t i = 0; i < ButtonVector.size(); i++)
            {
                ButtonVector[i].y_pos -= 50;
            }
        }
        NeedWindowRedraw = true;
    }

    if (Event->type == SDL_MOUSEBUTTONDOWN)
    {
        if (Event->button.button == 1)
        {
            LEFT_MOUSE_BUTTON_DRAG_POS[0] = Event->motion.x;
            LEFT_MOUSE_BUTTON_DRAG_POS[1] = Event->motion.y;
            // printf("[X,Y] %d,%d\n", Event->motion.x, Event->motion.y);
            for (size_t i = 0; i < ButtonVector.size(); i++)
            {
                int XPlusWidth = ButtonVector[i].x_pos + ButtonVector[i].width;
                int YPlusHeight = ButtonVector[i].y_pos + ButtonVector[i].height;
                // printf("XPlusWidth: %d\n", XPlusWidth);
                // printf("YPlusHeight: %d\n", YPlusHeight);
                if (Event->motion.x > ButtonVector[i].x_pos && Event->motion.x < XPlusWidth && Event->motion.y > ButtonVector[i].y_pos && Event->motion.y < YPlusHeight)
                {
                    SDL_Event ButtonEvent;
                    ButtonEvent.type = BUTTON_CLICKED_EVENT;
                    ButtonEvent.user.data1 = &ButtonVector[i].id;
                    SDL_PushEvent(&ButtonEvent);
                    // printf("[BUTTON_CLICKED_EVENT] PUSHED: %d\n", ButtonVector[i].id);
                }
            }
        }
        if (Event->button.button == 2)
        {
            RIGHT_MOUSE_BUTTON_DRAG_POS[0] = Event->motion.x;
            RIGHT_MOUSE_BUTTON_DRAG_POS[1] = Event->motion.y;
        }
    }

    if (Event->type == SDL_MOUSEBUTTONUP)
    {
        if (Event->button.button == 1)
        {
            LEFT_MOUSE_BUTTON_DRAG_POS[0] = 0;
            LEFT_MOUSE_BUTTON_DRAG_POS[1] = 0;
        }
        if (Event->button.button == 2)
        {
            RIGHT_MOUSE_BUTTON_DRAG_POS[0] = 0;
            RIGHT_MOUSE_BUTTON_DRAG_POS[1] = 0;
        }
    }

    if (Event->type == BUTTON_CLICKED_EVENT)
    {
        std::string id = *(std::string *)Event->user.data1;
        // printf("[BUTTON_CLICKED_EVENT] TRIGGERED: %s\n", id.c_str());

        for (size_t i = 0; i < ButtonVector.size(); i++)
        {
            if (ButtonVector[i].id == id)
            {
                if (ButtonVector[i].picked == true)
                {
                    ButtonVector[i].picked = false;
                }
                else
                    ButtonVector[i].picked = true;
            }
            else if (ButtonVector[i].id[0] == id[0] && ButtonVector[i].id[1] == id[1])
            {
                ButtonVector[i].picked = false;
            }
        }

        PointUpdate = true;
        NeedWindowRedraw = true;
        // printf("%d : %d\n", QuestionN, AnswerN);
    }

    if (Event->button.button == 1)
    {
        if (Event->type == SDL_MOUSEMOTION)
        {
            if (Event->motion.y != LEFT_MOUSE_BUTTON_DRAG_POS[1])
            {
                int C = LEFT_MOUSE_BUTTON_DRAG_POS[1] - Event->motion.y;
                BASE_Y_POS -= C;
                if (BASE_Y_POS > 10)
                {
                    int OLD_Y_BASE = BASE_Y_POS;
                    BASE_Y_POS = 10;
                    C = C + (OLD_Y_BASE - BASE_Y_POS);
                }
                LEFT_MOUSE_BUTTON_DRAG_POS[1] = Event->motion.y;
                for (size_t i = 0; i < ButtonVector.size(); i++)
                {
                    ButtonVector[i].y_pos -= C;
                }
                NeedWindowRedraw = true;
            }
            LEFT_MOUSE_BUTTON_DRAG_POS[0] = Event->motion.x;
        }
    }

    if (Event->button.button == 2)
    {
        if (Event->type == SDL_MOUSEMOTION)
        {
            RIGHT_MOUSE_BUTTON_DRAG_POS[0] = Event->motion.x;
            RIGHT_MOUSE_BUTTON_DRAG_POS[1] = Event->motion.y;
        }
    }
}

void Render()
{
    SDL_Delay(16);

    if (NeedWindowRedraw != true)
        return;

    SDL_RenderClear(Renderer);

    for (size_t i = 0; i < QuestionTextureVector.size(); i++)
    {
        renderQuestionTexture(QuestionTextureVector[i]);
    }

    if (PointUpdate)
        TotalPoint = 0;

    for (size_t i = 0; i < ButtonVector.size(); i++)
    {
        if (ButtonVector[i].picked)
        {
            renderTexture(PickedButtonT.texture, ButtonVector[i].x_pos - 3, ButtonVector[i].y_pos, PickedButtonT.width, PickedButtonT.height);

            if (PointUpdate)
            {
                char str_QuestionN[3];
                char str_AnswerN[2];
                bool s = false;
                for (size_t r = 0; r < strlen(ButtonVector[i].id.c_str()); r++)
                {
                    if (ButtonVector[i].id[r] == ':')
                    {
                        s = true;
                        continue;
                    }
                    if (s)
                    {
                        str_AnswerN[0] = ButtonVector[i].id[r];
                    }
                    else
                    {
                        str_QuestionN[r] = ButtonVector[i].id[r];
                    }
                }
                str_QuestionN[2] = '\0';
                str_AnswerN[1] = '\0';
                // printf("%s\n", str_QuestionN);
                int QuestionN = atoi(str_QuestionN);
                int AnswerN = atoi(str_AnswerN);
                bool m210 = false;
                for (size_t l = 0; l < 9; l++)
                {
                    // printf("%d == %d\n", multi210[l], QuestionN);
                    if (multi210[l] == QuestionN)
                        m210 = true;
                }
                if (m210)
                    TotalPoint += 210 * (AnswerN + 1);
                else
                    TotalPoint += 120 * (AnswerN + 1);
            }
        }
    }

    if (PointUpdate)
    {
        std::string TPS = std::to_string(TotalPoint);
        if (TotalPoint > 0)
        {
            switch (strlen(TPS.c_str()))
            {
            case 3:
            {
                char U[5] = {TPS[0], '.', TPS[1], TPS[2], '\0'};
                TotalPointT = createTexture(U, 2);
                break;
            }
            case 4:
            {
                char U[6] = {TPS[0], TPS[1], '.', TPS[2], TPS[3], '\0'};
                TotalPointT = createTexture(U, 2);
                break;
            }
            case 5:
            {
                char U[7] = {TPS[0], TPS[1], TPS[2], '.', TPS[3], TPS[4], '\0'};
                TotalPointT = createTexture(U, 2);
                break;
            }
            }
        }
        else
            TotalPointT = createTexture("0", 2);

        if (Category != 1 && TotalPoint > 0 && TotalPoint < 5001)
        {
            CategoryT = createTexture("SANGAT MISKIN", 2);
            Category = 1;
        }
        else if (Category != 2 && TotalPoint >= 5001 && TotalPoint <= 6667)
        {
            CategoryT = createTexture("MISKIN", 2);
            Category = 2;
        }
        else if (Category != 3 && TotalPoint > 6667 && TotalPoint <= 8333)
        {
            CategoryT = createTexture("HAMPIR MISKIN", 2);
            Category = 3;
        }
        else if (Category != 4 && TotalPoint > 8333)
        {
            CategoryT = createTexture("RENTAN MISKIN", 2);
            Category = 4;
        }

        PointUpdate = false;
    }

    renderTexture(TotalPointT.texture, BASE_X_POS + BASE_X_ANSWER_POS + QuestionTextureVector[QuestionTextureVector.size() - 1].x_pos + FONT_SIZE + 220, BASE_Y_POS + QuestionTextureVector[QuestionTextureVector.size() - 1].y_pos + 30, TotalPointT.width, TotalPointT.height);

    int w_w = 0;
    SDL_GetWindowSize(Window, &w_w, NULL);
    renderTexture(TotalPointT.texture, w_w - TotalPointT.width - 10, 5, TotalPointT.width, TotalPointT.height);

    if (Category > 0)
    {
        renderTexture(CategoryT.texture, BASE_X_POS + BASE_X_ANSWER_POS + QuestionTextureVector[QuestionTextureVector.size() - 1].x_pos + FONT_SIZE + 220, BASE_Y_POS + QuestionTextureVector[QuestionTextureVector.size() - 1].y_pos + 60, CategoryT.width, CategoryT.height);
        renderTexture(CategoryT.texture, w_w - CategoryT.width - 10, CategoryT.height + 5, CategoryT.width, CategoryT.height);
    }

    SDL_RenderPresent(Renderer);

    NeedWindowRedraw = false;
}

/**
 * @brief Deallocate memories and exit
 *
 */
void Exit_Program()
{
    TTF_CloseFont(Font);
    Font = NULL;

    TTF_Quit();

    for (size_t i = 0; i < QuestionTextureVector.size(); i++)
    {
        destroyQuestionTexture(QuestionTextureVector[i]);
    }

    SDL_DestroyTexture(TotalPointT.texture);
    TotalPointT.texture = NULL;
    TotalPointT.width = 0;
    TotalPointT.height = 0;
    TotalPoint = 0;

    SDL_DestroyTexture(TotalPointS.texture);
    TotalPointS.texture = NULL;
    TotalPointS.width = 0;
    TotalPointS.height = 0;

    SDL_DestroyTexture(PickedButtonT.texture);
    PickedButtonT.texture = NULL;
    PickedButtonT.width = 0;
    PickedButtonT.height = 0;

    SDL_DestroyTexture(CategoryT.texture);
    CategoryT.texture = NULL;
    CategoryT.width = 0;
    CategoryT.height = 0;

    SDL_DestroyRenderer(Renderer);
    Renderer = NULL;

    SDL_DestroyWindow(Window);
    Window = NULL;

    SDL_Quit();
}

int main(int argc, char *argv[])
{
    if (Init_Program() < 0)
    {
        printf("SDL ERROR: %s\n", SDL_GetError());
        return -1;
    }

    while (RETURN_EXIT == false)
    {
        SDL_Event Event;
        while (SDL_PollEvent(&Event))
        {
            HandleEvent(&Event);
        }

        Render();
    }

    Exit_Program();

    return 0;
}