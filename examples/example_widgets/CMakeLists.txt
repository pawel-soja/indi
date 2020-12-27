add_executable(example_widgets example_widgets.cpp
    widgets/widgeteventdispatcher.cpp
    widgets/widgets.cpp
    widgets/widget.cpp
    widgets/widgetdata.cpp
    widgets/widgetproperty.cpp
)

include_directories(widgets)

target_link_libraries(example_widgets indidriver)


