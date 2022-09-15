find_package(PythonInterp)

set(STYLE_FILTER)


function(add_style_check_target TARGET_NAME)
	
	if(NOT PYTHONINTERP_FOUND)
		return()
	endif()
    get_target_property(SOURCES_LIST ${TARGET_NAME} SOURCES)

	list(SORT SOURCES_LIST)
	list(REMOVE_DUPLICATES SOURCES_LIST)
    list(TRANSFORM SOURCES_LIST PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)

    
	add_custom_command(TARGET ${TARGET_NAME}
        PRE_BUILD
		COMMAND
			"cpplint"
			${SOURCES_LIST}
		DEPENDS ${SOURCES_LIST}
		VERBATIM
	)
	
endfunction(add_style_check_target)
