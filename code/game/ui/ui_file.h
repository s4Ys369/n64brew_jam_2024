#ifndef UI_FILE_H
#define UI_FILE_H

#ifdef __cplusplus
extern "C" {
#endif

const char* basePath = "rom:/game/ui/";

char* ui_filePath(const char* fn);

char* ui_filePath(const char* fn)
{
    char* fullPath = (char*)malloc(256 * sizeof(char));
    if (!fullPath) {
        return NULL;
    }

    sprintf(fullPath, "%s%s", basePath, fn);

    return fullPath;
}

const char* uiFontFileName[2];

void ui_fileFonts(void);

void ui_fileFonts(void)
{
    uiFontFileName[0] = ui_filePath("fonts/TitanOne-Regular.font64");
    uiFontFileName[1] = ui_filePath("fonts/chunkysans.font64");
}

const char* uiSpriteButtonFileName[6];
const char* uiSpritePanelFileName[6];

void ui_fileSprites(void);

void ui_fileSprites(void) {
    uiSpriteButtonFileName[0] = ui_filePath("buttons/control_stick.ia8.sprite");
    uiSpriteButtonFileName[1] = ui_filePath("buttons/d_pad_triggers.ia8.sprite");
    uiSpriteButtonFileName[2] = ui_filePath("buttons/c_buttons0.rgba32.sprite");
    uiSpriteButtonFileName[3] = ui_filePath("buttons/c_buttons1.rgba32.sprite");
    uiSpriteButtonFileName[4] = ui_filePath("buttons/face_buttons0.rgba32.sprite");
    uiSpriteButtonFileName[5] = ui_filePath("buttons/face_buttons1.rgba32.sprite");

    uiSpritePanelFileName[0] = ui_filePath("panels/border.ia4.sprite");
    uiSpritePanelFileName[1] = ui_filePath("panels/gloss.ia4.sprite");
    uiSpritePanelFileName[2] = ui_filePath("panels/gradient.ia4.sprite");
    uiSpritePanelFileName[3] = ui_filePath("panels/pattern_bubble_grid.ia4.sprite");
    uiSpritePanelFileName[4] = ui_filePath("panels/pattern_tessalate.ia4.sprite");
    uiSpritePanelFileName[5] = ui_filePath("panels/star.ia8.sprite");
}

void ui_fileGet(void);

void ui_fileGet(void)
{
    ui_fileFonts();
    ui_fileSprites();
}

void ui_fileCleanup(void)
{
    for (int i = 0; i < 2; i++)
        free((char*)uiFontFileName[i]);

    for (int i = 0; i < 6; i++) {
        free((char*)uiSpriteButtonFileName[i]);
        free((char*)uiSpritePanelFileName[i]);
    }
}

#ifdef __cplusplus
}
#endif

#endif // UI_FILE_H