set(UI_GENERATED_DIR "${CMAKE_CURRENT_LIST_DIR}/ui_generated")
set(UI_GENERATED_SOURCES "")
set(UI_INCLUDE_DIRS "ui_fallback")

if(EXISTS "${UI_GENERATED_DIR}/ui.c")
    list(APPEND UI_GENERATED_SOURCES "ui_generated/ui.c")
    list(APPEND UI_INCLUDE_DIRS "ui_generated")
    set(HAS_GENERATED_UI 1)
else()
    set(HAS_GENERATED_UI 0)
endif()

idf_component_register(
    SRCS
        "main.c"
        "ui_fallback/fallback_ui.c"
        ${UI_GENERATED_SOURCES}
    INCLUDE_DIRS
        ${UI_INCLUDE_DIRS}
    REQUIRES
        esp_lcd
        esp_lcd_touch_gt911
        esp_lvgl_port
        lvgl
)

target_compile_definitions(${COMPONENT_LIB} PRIVATE HAS_GENERATED_UI=${HAS_GENERATED_UI})
