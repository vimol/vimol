file(GLOB vimol_core_src
  atoms.c
  bind.c
  camera.c
  cmd.c
  edit.c
  error.c
  exec.c
  formats.c
  graph.c
  history.c
  main.c
  pair.c
  rec.c
  sel.c
  settings.c
  spi.c
  state.c
  statusbar.c
  sys.c
  tabs.c
  tok.c
  undo.c
  util.c
  vec.c
  view.c
  xmalloc.c
  yank.c
  )


  # Properties->C/C++->General->Additional Include Directories
  include_directories(${CAIRO_INCLUDE_DIRS})
  include_directories(${SDL2_INCLUDE_DIRS})

  # require cairo includes
  LINK_DIRECTORIES(${CAIRO_LIBRARY_DIRS})

  add_executable( ${PROJECT_NAME} main.c ${vimol_core_src}  )

  # Properties->Linker->Input->Additional Dependencies
  target_link_libraries( ${PROJECT_NAME} ${CAIRO_LIBRARIES} ${SDL2_LIBRARIES} )
