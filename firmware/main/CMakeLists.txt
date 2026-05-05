set(UI_GENERATED_DIR "${CMAKE_CURRENT_LIST_DIR}/ui_generated")
set(UI_INCLUDE_DIRS "ui_fallback")
set(GENERATED_UI_MODE 0)

file(GLOB UI_GENERATED_SOURCES CONFIGURE_DEPENDS "${UI_GENERATED_DIR}/*.c")

if(UI_GENERATED_SOURCES)
    list(APPEND UI_INCLUDE_DIRS "ui_generated")
    if(EXISTS "${UI_GENERATED_DIR}/ui.c")
        set(GENERATED_UI_MODE 1)
    elseif(EXISTS "${UI_GENERATED_DIR}/main_screen_gen.c")
        set(GENERATED_UI_MODE 2)
    endif()
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

target_compile_definitions(${COMPONENT_LIB} PRIVATE GENERATED_UI_MODE=${GENERATED_UI_MODE})
