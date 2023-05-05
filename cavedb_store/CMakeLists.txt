# cmake -D release=1 -D CMAKE_INSTALL_PREFIX=~/.local ..

#定义工程名及版本
project(cavedb_store)
set(version 1.0.0)
set(soversion 1.0)

cmake_minimum_required (VERSION 3.0.0) 
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS ON)
set(CMAKE_USE_RELATIVE_PATHS ON)
set(CMAKE_INCLUDE_DIRECTORIES_BEFORE ON)
set(CMAKE_BUILE_TYPE DEBUG) 


#导入一此需要用到的工具包
include(CheckTypeSize)
include(CheckFunctionExists)
include(CheckIncludeFileCXX)
include(CheckIncludeFiles)
include(CheckLibraryExists)
include(CheckCSourceCompiles)
include(FindPkgConfig)
include(UsePkgConfig)
enable_testing()

#定制编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS} -Wall -fPIC -Bsymbolic")

if(release)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -DNDEBUG ")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden -std=c99 -O2 -DNDEBUG ")
	set(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS} -O2")
	message("Build as release")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_DEBUG -g -fstack-protector ")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden -D_DEBUG -std=c99 -g -fstack-protector ")
	set(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS} -g -fstack-protector ")
	message("Build as debug")
endif()

#定义安装目录
set(CMAKE_INSTALL_PREFIX $ENV{HOME}/.local)

#搜索源代码
aux_source_directory(src src_cpps)

#搜索头文件
file(GLOB src_hs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.h)
include_directories(src)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)




#根据外部依赖的存在与定义一些宏
foreach(v ${BUILD_DEPENDS})
	set(CMAKE_BUILD_DEPENDS "${CMAKE_BUILD_DEPENDS} ${v}")
endforeach()

foreach(v ${RUN_DEPENDS})
	set(CMAKE_RUN_DEPENDS "${CMAKE_RUN_DEPENDS} ${v}")
endforeach()

foreach(v ${INCS_DEPENDS})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${v}")
endforeach()

set(CMAKE_LIBS_DEPENDS ${LIBS_DEPENDS})

foreach(v ${CONFIG_MACROS})
	set(CMAKE_CONFIG_MACROS "${CMAKE_CONFIG_MACROS}#define ${v}\n")
endforeach()

#修改配置文件
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/config.h)


#目标动态库
add_library(${PROJECT_NAME} ${share_type} ${src_cpps} ${src_hs})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}")
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${version} SOVERSION ${soversion})

#拷贝文件
file(COPY ${src_hs} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include/)

#链接库
target_link_libraries(${PROJECT_NAME} -lpthread ${CMAKE_LIBS_DEPENDS})

#定义应用目标
file(GLOB src_app RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/app/*.cc)
foreach(var ${src_app})
	string(REGEX REPLACE "src/app/(.*).cc" "\\1" var2 ${var})
	add_executable(${var2} ${var})
	target_link_libraries(${var2} ${PROJECT_NAME})
	#target_link_libraries(${var2} -L/usr/lib64/mysql -lmysqlclient)
	#install(TARGETS ${PROJECT_NAME} DESTINATION ${install_dir})
endforeach()

#定义安装目标
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/pkg/${PROJECT_NAME}.pc DESTINATION lib64/pkgconfig)
install(FILES ${src_hs} DESTINATION include/cavedb_store)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/include/config.h DESTINATION include/cavedb_store)
install(FILES ${src_hs_store} DESTINATION include/cavedb_store/store/)

install(TARGETS ${PROJECT_NAME} DESTINATION lib64)
