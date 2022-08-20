project('VMInstallerCLI','c')
PROJECT_NAME = meson.project_name()
local_dir = meson.current_source_dir()

message('initial definitions complete')

env = environment()
#env.set(local_dir, 0)
run_command('bash', '@0@/generate_gresource_xml.sh'.format(local_dir), PROJECT_NAME, 'ui_file_list.txt', env: env)

message('run command comlete')

gtkdep = dependency('gtk4')
#glibcdep = dependency('glibc')

#needed for math library
add_project_link_arguments('-lm', language : 'c')


gnome = import('gnome')
resources = gnome.compile_resources('resources','@0@.gresource.xml'.format(PROJECT_NAME))


message('gresources complete')

#install_data('builder.ui', install_dir: pkgdatadir)


sourcefiles = files('main.c')


executable(PROJECT_NAME, sourcefiles, dependencies: gtkdep, c_args:[ '-DLOCAL_DIR="@0@/"'.format(local_dir)], export_dynamic: true)
