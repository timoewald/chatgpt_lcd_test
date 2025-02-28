#ifndef LV_INIT_H
#define LV_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialisiert LVGL, richtet den Zeichnungspuffer ein und
 *        registriert den Display-Treiber.
 */
void lvgl_init(void);

#ifdef __cplusplus
}
#endif

#endif // LV_INIT_H
