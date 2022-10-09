#include <gtk/gtk.h>
#include <string.h>


#define APPLICATION_ID "com.github.maxbrandtner.VMInstallerCLI"
#define APPLICATION_URI "/com/github/maxbrandtner/VMInstallerCLI/"

#define UI_DIR_WINDOW APPLICATION_URI "window.ui"



char *exec_dir;
unsigned int flag = 0;
GtkWidget *stack;
char *selected_iso_filename;
unsigned int cpu_cores = 6;
unsigned int cpu_threads = 2;
unsigned int selected_cpu_cores = 1;
unsigned int selected_cpu_threads = 1;
unsigned int n_gpus = 1;
double max_free_storage = 200;
double max_memory = 16;
char** gpu_names;
unsigned int selected_graphics_mode = 0;
double selected_storage = 0;
double selected_memory = 0;
unsigned int selected_bios = 0;
unsigned int selected_tpm = 0;
char * selected_rom_filename;
GtkWidget *iso_icon;
GtkWidget *iso_filename_label;
GtkWidget *romfile_icon;
GtkWidget *romfile_filename_label;
char *vm_name;
unsigned int user_group_active = 0;
unsigned int created_vm_name_output_file = 0;
unsigned int position_buffer = 0;
GtkButton *button_previous;
GtkButton *button_next;



char* FilterArray(char* input_array){

	int CharCheck(char input){
		int dec_input = (int)input;

		if((dec_input <= 31)||(dec_input >= 48 && dec_input <= 57)||(dec_input >= 65 && dec_input <= 90)||(dec_input >= 97 && dec_input <= 122)||(dec_input == 45)||(dec_input == 95)||(dec_input == 127)||(dec_input == 255)){

			return 1;
		}else{

			return 0;
		}
	}


	unsigned int invalid_chars = 0;

	char* buffer_array = calloc(strlen(input_array), sizeof(char));


	for(int i = 0; i < strlen(input_array); i ++){
		if(CharCheck(input_array[i]) == 1){
			buffer_array[i - invalid_chars] = input_array[i];

		}else{
			invalid_chars ++;
		}
	}

	return buffer_array;
}



void switch_to_next(GtkWidget *stack){
	if(selected_graphics_mode < 3 && flag == 3){
		flag = 4;
	}

	unsigned int changed_visible_stack_child = 0;


	if(flag == 0){
		gtk_stack_set_visible_child_name(GTK_STACK(stack), "choose_vm_name");
		changed_visible_stack_child = 1;

	}else if(flag == 1){
		if(vm_name != NULL){
			gtk_stack_set_visible_child_name(GTK_STACK(stack), "select_iso_file");
			changed_visible_stack_child = 1;
		}

	}else if(flag == 2){
		if(selected_iso_filename != NULL){
			gtk_stack_set_visible_child_name(GTK_STACK(stack), "install_settings_box");
			changed_visible_stack_child = 1;
		}

	}else if(flag == 3){
		gtk_stack_set_visible_child_name(GTK_STACK(stack), "select_romfile_box");
		changed_visible_stack_child = 1;

	}else if(flag == 4){
		gtk_stack_set_visible_child_name(GTK_STACK(stack), "bios_options_box");
		changed_visible_stack_child = 1;

	}else if(flag == 5){
		gtk_stack_set_visible_child_name(GTK_STACK(stack), "install_vm_box");
		changed_visible_stack_child = 1;
	}

	if(changed_visible_stack_child == 1){
		flag ++;

	}
}

void switch_to_previous(GtkWidget *stack){
	if(selected_graphics_mode < 3 && flag == 5){
                flag = 4;
        }


        if(flag == 1){
		gtk_stack_set_visible_child_name(GTK_STACK(stack), "start_installer");

	}else if(flag == 2){
		gtk_stack_set_visible_child_name(GTK_STACK(stack), "choose_vm_name");


	}else if(flag == 3){
		gtk_stack_set_visible_child_name(GTK_STACK(stack), "select_iso_file");

	}else if(flag == 4){
		gtk_stack_set_visible_child_name(GTK_STACK(stack), "install_settings_box");

	}else if(flag == 5){
		gtk_stack_set_visible_child_name(GTK_STACK(stack), "select_romfile_box");

	}else if(flag == 6){
		gtk_stack_set_visible_child_name(GTK_STACK(stack), "bios_options_box");
	}

        if(flag > 0){
                flag --;
	}
}



void changed_vm_name_callback(GtkWidget *entry){
	GtkEntryBuffer *entry_buffer = gtk_entry_get_buffer(GTK_ENTRY(entry));
	char* name_buffer = (char*)gtk_entry_buffer_get_text(GTK_ENTRY_BUFFER(entry_buffer));

	name_buffer = FilterArray(name_buffer);

	//gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER(entry_buffer), name_buffer, strlen(name_buffer));
	GtkEntryBuffer *entry_buffer_filtered = gtk_entry_buffer_new(name_buffer, strlen(name_buffer));

  int entry_position = gtk_editable_get_position(GTK_EDITABLE(entry));
	gtk_entry_set_buffer(GTK_ENTRY(entry), GTK_ENTRY_BUFFER(entry_buffer_filtered));
  gtk_editable_set_position(GTK_EDITABLE(entry), entry_position);


	//vm_name = name_buffer;
	
	{
		char *name_equal_check_buffer = calloc(BUFSIZ + 1, sizeof(char));

		if(user_group_active == 1){
			strcpy(name_equal_check_buffer, "virsh --connect qemu:///system list --all | awk '{print $2}' | grep -w ");

		}else {
			char *create_tmp_dir_buffer = calloc(BUFSIZ + 1, sizeof(char));

			strcpy(create_tmp_dir_buffer, exec_dir);
			strcat(create_tmp_dir_buffer, " cd ../bash-scripts; mkdir -p .tmp");

		}


		strcat(name_equal_check_buffer, name_buffer);

		int status = 0;

		if(strcmp(name_buffer, "")){
			status = system(name_equal_check_buffer);
		}

		if(status != 0){
			vm_name = name_buffer;

			if(strcmp(vm_name, "")){
        gtk_widget_set_can_target(GTK_WIDGET(button_next), true);
				gtk_entry_set_icon_from_icon_name(GTK_ENTRY(entry), GTK_ENTRY_ICON_SECONDARY, "emblem-ok-symbolic");
				gtk_entry_set_icon_tooltip_text(GTK_ENTRY(entry), GTK_ENTRY_ICON_SECONDARY, "VM name available");
			}else{
        gtk_widget_set_can_target(GTK_WIDGET(button_next), false);
				gtk_entry_set_icon_from_icon_name(GTK_ENTRY(entry), GTK_ENTRY_ICON_SECONDARY, "emblem-important-symbolic");
				gtk_entry_set_icon_tooltip_text(GTK_ENTRY(entry), GTK_ENTRY_ICON_SECONDARY, "must enter VM name");
			}
		}else{
			vm_name = NULL;
      gtk_widget_set_can_target(GTK_WIDGET(button_next), false);
			//gtk_widget_set_tooltip_text(GTK_WIDGET(entry), "VM already exists");
			//gtk_tooltip_set_icon_from_gicon();
			gtk_entry_set_icon_from_icon_name(GTK_ENTRY(entry), GTK_ENTRY_ICON_SECONDARY, "emblem-important-symbolic");
			gtk_entry_set_icon_tooltip_text(GTK_ENTRY(entry), GTK_ENTRY_ICON_SECONDARY, "VM already exists");
		}
	}
}


void store_filename(GtkWidget *dialog, gint response, GtkFileChooser *chooser){

	if(response == GTK_RESPONSE_ACCEPT){

		GFile *file = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(dialog));

		char *path = g_file_get_path(file);

		selected_iso_filename = path;
		//directory = gtk_file_chooser_get_current_folder(chooser);
		//selected_iso_filename = gtk_file_chooser_get_current_name(chooser);
		//g_print("You selected %s\n", selected_iso_filename);

		switch_to_next(stack);
	}
}


void iso_file_icon_callback(GtkWidget *dialog, gint response, GtkWidget *icon){
	if(response == GTK_RESPONSE_ACCEPT){
        	gtk_image_set_from_icon_name(GTK_IMAGE(icon), "document-save");
	}
}



void iso_filename_label_callback(GtkWidget *dialog, gint response, GtkWidget *label){
	if(response == GTK_RESPONSE_ACCEPT){
		gtk_label_set_text(GTK_LABEL(label), (const char*)selected_iso_filename);
	}
}




void choose_iso_file(GtkWidget *window){
	GtkWidget *dialog = gtk_file_chooser_dialog_new("Select iso file", GTK_WINDOW(window),
			GTK_FILE_CHOOSER_ACTION_OPEN,
			"Cancel", GTK_RESPONSE_CANCEL,
			"Select", GTK_RESPONSE_ACCEPT, NULL);

	GtkFileFilter *filter = gtk_file_filter_new();
	gtk_file_filter_add_suffix(filter, "iso");

	gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(dialog), filter);


	g_signal_connect(dialog, "response", G_CALLBACK(store_filename), GTK_FILE_CHOOSER(dialog));
	g_signal_connect(dialog, "response", G_CALLBACK(gtk_window_destroy), GTK_WINDOW(dialog));
	g_signal_connect(dialog, "response", G_CALLBACK(iso_file_icon_callback), iso_icon);
	g_signal_connect(dialog, "response", G_CALLBACK(iso_filename_label_callback), iso_filename_label);

	//GtkWidget *file_selector = gtk_file_selection_new("Select an iso file");

	//g_signal_connect(GTK_FILE_SELECTION(file_selector)->ok_button, "clicked", G_CALLBACK(store_filename), file_selector);
	//g_signal_connect_swapped(GTK_FILE_SELECTION(file_selector)->ok_button, "clicked", G_CALLBACK(gtk_widget_destroy), file_selector);
	//g_signal_connect_swapped(GTK_FILE_SELECTION(file_selector)->cancel_button, "clicked", G_CALLBACK(gtk_widget_destory), file_selector);


	gtk_widget_show(dialog);
}



void cpu_selected_cores_callback(GtkWidget *scale){
        selected_cpu_cores = gtk_range_get_value(GTK_RANGE(scale));
}




void cpu_selected_threads_callback(GtkWidget *scale){
	selected_cpu_threads = gtk_range_get_value(GTK_RANGE(scale));
}



//from gtk3-demos
GtkWidget *combo_box_new_from_strings(const char **string_input, unsigned int n_strings){

	GtkWidget *combo_box = gtk_combo_box_text_new();


	for(int i = 0; i < n_strings; i++){
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), string_input[i]);
	}

	gtk_combo_box_set_active(GTK_COMBO_BOX(combo_box), 0);


	return combo_box;

}



void changed_selected_graphics_callback(GtkWidget *combo_box){

	selected_graphics_mode = gtk_combo_box_get_active(GTK_COMBO_BOX(combo_box));
}



void storage_selected_callback(GtkWidget *scale){
	selected_storage = gtk_range_get_value(GTK_RANGE(scale));
}



void memory_selected_callback(GtkWidget *scale){
	selected_memory = gtk_range_get_value(GTK_RANGE(scale));
}



void changed_selected_bios_callback(GtkWidget *combo_box){
	selected_bios = gtk_combo_box_get_active(GTK_COMBO_BOX(combo_box));
}



void changed_selected_tpm_callback(GtkWidget *checkbutton){
	selected_tpm = (int)gtk_check_button_get_active(GTK_CHECK_BUTTON(checkbutton));
}



void romfile_icon_callback(GtkWidget *dialog, gint response, GtkWidget *icon){
        if(response == GTK_RESPONSE_ACCEPT){
                gtk_image_set_from_icon_name(GTK_IMAGE(icon), "document-save");
        }
}



void romfile_filename_label_callback(GtkWidget *dialog, gint response, GtkWidget *label){
        if(response == GTK_RESPONSE_ACCEPT){
                gtk_label_set_text(GTK_LABEL(label), (const char*)selected_iso_filename);
        }
}



void store_rom_filename(GtkWidget *dialog, gint response, GtkFileChooser *chooser){

	if(response == GTK_RESPONSE_ACCEPT){

		GFile *file = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(dialog));

		char *path = g_file_get_path(file);

		selected_rom_filename = path;
		//directory = gtk_file_chooser_get_current_folder(chooser);
		//selected_iso_filename = gtk_file_chooser_get_current_name(chooser);
		//g_print("You selected %s\n", selected_iso_filename);

		switch_to_next(stack);
	}
}




void choose_romfile_callback(GtkWidget *window){
        GtkWidget *dialog = gtk_file_chooser_dialog_new("Select romfile", GTK_WINDOW(window),
                        GTK_FILE_CHOOSER_ACTION_OPEN,
                        "Cancel", GTK_RESPONSE_CANCEL,
                        "Select", GTK_RESPONSE_ACCEPT, NULL);

        GtkFileFilter *filter = gtk_file_filter_new();
        gtk_file_filter_add_suffix(filter, "rom");

        gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(dialog), filter);


        g_signal_connect(dialog, "response", G_CALLBACK(store_rom_filename), GTK_FILE_CHOOSER(dialog));
        g_signal_connect(dialog, "response", G_CALLBACK(gtk_window_destroy), GTK_WINDOW(dialog));
        g_signal_connect(dialog, "response", G_CALLBACK(romfile_icon_callback), romfile_icon);
        g_signal_connect(dialog, "response", G_CALLBACK(romfile_filename_label_callback), romfile_filename_label);

        //GtkWidget *file_selector = gtk_file_selection_new("Select an iso file");

        //g_signal_connect(GTK_FILE_SELECTION(file_selector)->ok_button, "clicked", G_CALLBACK(store_filename), file_selector);
        //g_signal_connect_swapped(GTK_FILE_SELECTION(file_selector)->ok_button, "clicked", G_CALLBACK(gtk_widget_destroy), file_selector);
        //g_signal_connect_swapped(GTK_FILE_SELECTION(file_selector)->cancel_button, "clicked", G_CALLBACK(gtk_widget_destory), file_selector);


        gtk_widget_show(dialog);
}





static void install_vm(){
	char *exec_string;
	//char* exec_string = malloc(10000);
	//exec_string = "pkexec bash bash-scripts/main.sh";
	//exec_string = (char *) malloc(strlen("pkexec bash bash-scripts/main.sh") + strlen(vm_name) + strlen(selected_iso_filename) + (int)log10(selected_cpu_sockets) + (int)log10(selected_cpu_cores) *  + (int)log10(selected_cpu_threads) + (int)log10(selected_memory) + (int)log10(selected_storage) + strlen(selected_rom_filename) + sizeof(int) * 100);
	exec_string = (char *) calloc(BUFSIZ + 1, sizeof(char));
	//printf("%ld\n", strlen(exec_string));
	//exec_string = realloc(exec_string, 10000);

	//exec_string = NULL;

	char* selected_cpu_cores_string = malloc((int)log10(selected_cpu_cores));
	char* selected_cpu_threads_string = malloc((int)log10(selected_cpu_threads));
	//char* selected_memory_string = malloc((int)log10((int)selected_memory));
	char* selected_memory_string = calloc(100, sizeof(char));
  //char* selected_storage_string = malloc((int)log10(selected_storage));
	char* selected_storage_string = calloc(100, sizeof(char));
  char* selected_paravirtualized_string = malloc(sizeof(char)*2);
	char* selected_graphics_mode_string = malloc((int)log10(selected_graphics_mode - 2));
	char* selected_secboot_string = malloc(sizeof(char) *2);
	char* selected_tpm_string = malloc(sizeof(char) * 5);


	sprintf(selected_cpu_cores_string, "%d", selected_cpu_cores);

	sprintf(selected_cpu_threads_string, "%d", selected_cpu_threads);

	sprintf(selected_memory_string, "%d", (int)selected_memory);

	sprintf(selected_storage_string, "%d", (int)selected_storage);

	if(selected_graphics_mode >= 3){
		sprintf(selected_paravirtualized_string, "1");

	}else{
		sprintf(selected_paravirtualized_string, "0");
	}

	if(selected_graphics_mode >= 3){
		sprintf(selected_graphics_mode_string, " %d", selected_graphics_mode - 2);

	}else{
		sprintf(selected_graphics_mode_string, "0");
	}

	if(selected_bios == 1){
		sprintf(selected_secboot_string, "1");

	}else{
		sprintf(selected_secboot_string, "0");
	}

	if(selected_tpm == 1){
		sprintf(selected_tpm_string, "1");
		//printf("selected_tmp=1\n");

	}else{
		//printf("error occurs here!\n");
		sprintf(selected_tpm_string, "0");
		//printf("selected_tpm!=1\n");
	};

	strcat(exec_string, exec_dir);
  strcat(exec_string, "cd ../bash-scripts&&");
	strcat(exec_string,"pkexec --keep-cwd bash ../bash-scripts/main.sh");
	strcat(exec_string, " \"");
	strcat(exec_string, vm_name);
	strcat(exec_string, "\" \"");
	strcat(exec_string, selected_iso_filename);
	strcat(exec_string, "\" ");
	strcat(exec_string, selected_cpu_cores_string);
	strcat(exec_string, " ");
	strcat(exec_string, selected_cpu_threads_string);
	strcat(exec_string, " ");
	strcat(exec_string, selected_memory_string);
	strcat(exec_string, " ");
	strcat(exec_string, selected_storage_string);
	strcat(exec_string, " ");
	strcat(exec_string, selected_paravirtualized_string);
	strcat(exec_string, " ");
	strcat(exec_string, selected_graphics_mode_string);
	strcat(exec_string, " \"");
	strcat(exec_string, selected_rom_filename);
	strcat(exec_string, "\" ");
	strcat(exec_string, selected_secboot_string);
	strcat(exec_string, " ");
	strcat(exec_string, selected_tpm_string);
	strcat(exec_string, " ");

	printf("%s\n", exec_string);


	system(exec_string);


	free(exec_string);
}






static void app_startup(GApplication *application){


  //char self[PATH_MAX] = { 0 };
	char* exe_dir;
	//exe_dir = malloc(PATH_MAX + 1);
	exe_dir = (char*)calloc(PATH_MAX, sizeof(char));
	readlink("/proc/self/exe", exe_dir, PATH_MAX);
	//strcat(exe_dir, "\n");

	char* cmd_cd;
	cmd_cd = malloc(PATH_MAX + 1000);

	char *xargs_string = "| xargs dirname)";
	//| sed \"s/^/\\\"/\" | sed \"s/$/\\\"/\")";

	strcpy(cmd_cd, "cd $(echo ");
	strcat(cmd_cd, exe_dir);
	strcat(cmd_cd, xargs_string);
	strcat(cmd_cd, "&&");

	//printf("%s\n", cmd_cd);
	exec_dir = (char*)calloc(PATH_MAX, sizeof(char));
	strcpy(exec_dir, cmd_cd);


	//cpu_cores
	{
		char *buffer = NULL;
		buffer = (char*)calloc((long unsigned int)buffer, BUFSIZ + 1);

		FILE *content_buffer = popen("lscpu | grep 'Core(s)' | awk '{print $4}'","r");

		if(content_buffer != NULL){
			int chars_read = fread(buffer, sizeof(char), BUFSIZ, content_buffer);

			if(chars_read > 0){
				cpu_cores = atoi(buffer);
			}
		}

		pclose(content_buffer);
	}

	//printf("check cpu_cores\n");
	//printf("%d\n", cpu_cores);

	//cpu_threads
	{
		char *buffer = NULL;
		buffer = (char*)calloc((long unsigned int)buffer, BUFSIZ + 1);

                FILE *content_buffer = popen("lscpu | grep 'Thread(s)' | awk '{print $4}'","r");

                if(content_buffer != NULL){
                        int chars_read = fread(buffer, sizeof(char), BUFSIZ, content_buffer);

                        if(chars_read > 0){
                                cpu_threads = atoi(buffer);
                        }
                }

                pclose(content_buffer);
	}


	//printf("check cpu_threads\n");
	//printf("%d\n", cpu_threads);


	//max_free_storage
	{
		char *buffer = NULL;
		buffer = (char*)calloc((long unsigned int)buffer, BUFSIZ + 1);

								char *storage_script_string;
								storage_script_string = (char*)calloc(BUFSIZ, sizeof(char));

								strcpy(storage_script_string, exec_dir);
								strcat(storage_script_string, "bash ../bash-scripts/free_storage_main_device.sh");

								FILE *content_buffer = popen(storage_script_string,"r");

                if(content_buffer != NULL){
                       	int chars_read = fread(buffer, sizeof(char), BUFSIZ, content_buffer);

                        if(chars_read > 0){
                                max_free_storage = (float)atoi(buffer);
                       	}
                }

                pclose(content_buffer);
	}

	//printf("check max_free_storage\n");
	//printf("%f\n", max_free_storage);


	//max_memory
	{
	       	char *buffer = NULL;
		buffer = (char*)calloc((long unsigned int)buffer, BUFSIZ + 1);

                FILE *content_buffer = popen("echo $(($(cat /proc/meminfo | grep MemTotal | awk '{print $2}')))","r");

                if(content_buffer != NULL){
                        int chars_read = fread(buffer, sizeof(char), BUFSIZ, content_buffer);

                        if(chars_read > 0){
                                max_memory = atoi(buffer) / 1000;
                        }
                }

                pclose(content_buffer);
	}

	//printf("check max_memory\n");
	//printf("%f\n", max_memory);

	//n_gpus
	{
		char *buffer = NULL;
		buffer = (char*)calloc((long unsigned int)buffer, BUFSIZ + 1);

                FILE *content_buffer = popen("lspci -nn | grep VGA | wc -l","r");

                if(content_buffer != NULL){
                        int chars_read = fread(buffer, sizeof(char), BUFSIZ, content_buffer);

                        if(chars_read > 0){
                                n_gpus = atoi(buffer);
                        }
                }

                pclose(content_buffer);
	}

	//printf("check n_gpus\n");
	//printf("%d\n",n_gpus);
	//*gpu_names = (char**)realloc(*gpu_names, n_gpus * sizeof(char*));
	gpu_names = malloc(sizeof(char*) * n_gpus);
	//gpu_names
	{
		for(int i = 0; i < n_gpus; i ++){
			char *buffer;
			buffer = (char*)calloc(BUFSIZ + 1, sizeof(char));

			char *input_string;
			input_string = (char*)calloc(BUFSIZ + 1, sizeof(char));
			//input_string = "bash bash-scripts/list_gpus.sh ";
			//char *i_char = malloc(sizeof(char) * (long unsigned int)log10 * i);
			//sprintf(i_char, "%d", i);
			//printf("check 0\n");
			//strcat(input_string, i_char);
			//printf("%s\n", input_string);
			strcat(input_string, exec_dir);
			char* bash_script;
			bash_script = (char*)calloc(BUFSIZ + 1, sizeof(char));
			sprintf(bash_script, "bash ../bash-scripts/list_gpus.sh %i",(i + 1));
			strcat(input_string, bash_script);
			//printf("%s\n", input_string);


                	FILE *content_buffer = popen(input_string,"r");
									//FILE *content_buffer = popen("bash bash-scripts/list_gpus.sh 1", "r");

									//printf("check\n");
                	if(content_buffer != NULL){
													//printf("check again\n");
                       		int chars_read = fread(buffer, sizeof(char), BUFSIZ, content_buffer);
													//printf("check");
													gpu_names[i] = NULL;
                        	if(chars_read > 0){
																	//(*gpu_names)[i] = (char*)realloc((*gpu_names[i]), strlen(buffer) * sizeof(char));
                               		gpu_names[i] = malloc(sizeof(char) * strlen(buffer));
																	//printf("check alloc\n");
																	strcpy(gpu_names[i], buffer);
																	//int new_line_position = strcspn(gpu_names[i], "\n");
																	strtok(gpu_names[i], "\n");
																	//printf("check assign\n");
                        	}
               		}

                	//pclose(content_buffer);
		}
	}

	//printf("check gpu_names\n");

	/*for(int i = 0; i < n_gpus; i ++){
		printf("%s\n", gpu_names[i]);
	}*/

  char *groups_check = calloc(BUFSIZ + 1, sizeof(char));

  strcpy(groups_check ,exec_dir);
	strcat(groups_check ,"bash ../bash-scripts/user_groups_check.sh");

	int status = system(groups_check);

  if(status == 0){
		user_group_active = 1;
	}
}






static void app_activate(GApplication *application){
	GtkApplication *app = GTK_APPLICATION(application);

	GtkBuilder *build = gtk_builder_new_from_resource(UI_DIR_WINDOW);

	GtkWidget  *window  = GTK_WIDGET(gtk_builder_get_object(build, "window"));
	//GtkHeaderBar *headerbar = GTK_HEADER_BAR(gtk_builder_get_object(build, "headerbar"));
	//GtkButton *button_previous = GTK_BUTTON(gtk_builder_get_object(build, "button_previous"));
	//GtkButton *button_next = GTK_BUTTON(gtk_builder_get_object(build, "button_next"));




	g_object_unref(build);

	gtk_window_set_application(GTK_WINDOW(window), app);


	stack = gtk_stack_new();

	gtk_stack_set_transition_type(GTK_STACK(stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
	gtk_stack_set_transition_duration(GTK_STACK(stack), 500);

        gtk_window_set_child(GTK_WINDOW(window), stack);



	GtkWidget *headerbar = gtk_header_bar_new();


	button_previous = GTK_BUTTON(gtk_button_new_from_icon_name("go-previous-symbolic"));

	g_signal_connect_swapped(button_previous, "clicked", G_CALLBACK(switch_to_previous), stack);

	gtk_header_bar_pack_start(GTK_HEADER_BAR(headerbar), GTK_WIDGET(button_previous));



	button_next = GTK_BUTTON(gtk_button_new_from_icon_name("go-next-symbolic"));

	g_signal_connect_swapped(button_next, "clicked", G_CALLBACK(switch_to_next), stack);

	gtk_header_bar_pack_end(GTK_HEADER_BAR(headerbar), GTK_WIDGET(button_next));



        GtkBox *box_start_installer = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 20));
	gtk_widget_set_halign(GTK_WIDGET(box_start_installer), GTK_ALIGN_CENTER);
	gtk_widget_set_valign(GTK_WIDGET(box_start_installer), GTK_ALIGN_CENTER);

        gtk_stack_add_named(GTK_STACK(stack), GTK_WIDGET(box_start_installer), "start_installer");



	{
		GtkWidget *start_label = gtk_label_new("VM Installer CLI utility");

		gtk_box_append(GTK_BOX(box_start_installer), start_label);


		GtkButton *start_button = GTK_BUTTON(gtk_button_new_with_label("Start"));

		g_signal_connect_swapped(start_button, "clicked", G_CALLBACK(switch_to_next), stack);

		gtk_box_append(GTK_BOX(box_start_installer), GTK_WIDGET(start_button));
	}


	GtkBox *box_choose_vm_name = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 20));
	gtk_widget_set_halign(GTK_WIDGET(box_choose_vm_name), GTK_ALIGN_CENTER);
	gtk_widget_set_valign(GTK_WIDGET(box_choose_vm_name), GTK_ALIGN_CENTER);

	gtk_stack_add_named(GTK_STACK(stack), GTK_WIDGET(box_choose_vm_name), "choose_vm_name");

	{
		GtkWidget *choose_vm_name_label = gtk_label_new("Choose VM name");
		GtkWidget *choose_vm_name_entry = gtk_entry_new();
		//gtk_widget_set_has_tooltip(GTK_WIDGET(choose_vm_name_entry), true);
		gtk_entry_set_max_length(GTK_ENTRY(choose_vm_name_entry), 255);
		gtk_entry_set_placeholder_text(GTK_ENTRY(choose_vm_name_entry), "VM Name...");

		gtk_entry_set_icon_from_icon_name(GTK_ENTRY(choose_vm_name_entry), GTK_ENTRY_ICON_SECONDARY, "emblem-important-symbolic");
		gtk_entry_set_icon_tooltip_text(GTK_ENTRY(choose_vm_name_entry), GTK_ENTRY_ICON_SECONDARY, "must enter VM name");

		g_signal_connect_swapped(choose_vm_name_entry, "changed", G_CALLBACK(changed_vm_name_callback), choose_vm_name_entry);


		gtk_box_append(GTK_BOX(box_choose_vm_name), GTK_WIDGET(choose_vm_name_label));
		gtk_box_append(GTK_BOX(box_choose_vm_name), GTK_WIDGET(choose_vm_name_entry));
	}


	GtkBox *box_select_iso_file = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 20));
        gtk_widget_set_halign(GTK_WIDGET(box_select_iso_file), GTK_ALIGN_CENTER);
        gtk_widget_set_valign(GTK_WIDGET(box_select_iso_file), GTK_ALIGN_CENTER);

        gtk_stack_add_named(GTK_STACK(stack), GTK_WIDGET(box_select_iso_file), "select_iso_file");


	{
		iso_icon = gtk_image_new_from_icon_name("document-new");
		iso_filename_label = gtk_label_new("");
		gtk_image_set_icon_size(GTK_IMAGE(iso_icon), GTK_ICON_SIZE_LARGE);

		//GtkWidget *file_label = gtk_label_new("");

		GtkButton *choose_iso_button = GTK_BUTTON(gtk_button_new_with_label("Select an iso file"));

		g_signal_connect_swapped(choose_iso_button, "clicked", G_CALLBACK(choose_iso_file), window);
		//g_signal_connect_swapped(choose_iso_button, "clicked", G_CALLBACK(iso_file_icon_callback), iso_image);
		//g_signal_connect_swapped(choose_iso_button, "clicked", G_CALLBACK(iso_file_label_callback), file_label);

		gtk_box_append(GTK_BOX(box_select_iso_file), GTK_WIDGET(iso_icon));
		gtk_box_append(GTK_BOX(box_select_iso_file), GTK_WIDGET(iso_filename_label));
		gtk_box_append(GTK_BOX(box_select_iso_file), GTK_WIDGET(choose_iso_button));
	}



	GtkBox *install_settings_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 100));
	gtk_widget_set_halign(GTK_WIDGET(install_settings_box), GTK_ALIGN_CENTER);
	gtk_widget_set_valign(GTK_WIDGET(install_settings_box), GTK_ALIGN_CENTER);
	//gtk_grid_layout_set_column_spacing(GTK_GRID_LAYOUT(install_settings_grid), 25);
	//gtk_grid_layout_set_row_spacing(GTK_GRID_LAYOUT(install_settings_grid), 25);
	//gtk_grid_layout_set_column_homogeneous(GTK_GRID_LAYOUT(install_settings_grid), true);
	//gtk_grid_layout_set_row_homogeneous(GTK_GRID_LAYOUT(install_settings_grid), true);

	gtk_stack_add_named(GTK_STACK(stack), GTK_WIDGET(install_settings_box), "install_settings_box");

	{
		GtkBox *box_coloum_1 = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 50));

		//cpu
		{
			GtkBox *box_cpu = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 10));
			gtk_widget_set_halign(GTK_WIDGET(box_cpu), GTK_ALIGN_BASELINE);

			GtkWidget *cpu_TopLevelLabel = gtk_label_new("CPU                                                            ");







			GtkBox *box_cpu_cores = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 5));
                        GtkWidget *cpu_cores_label = gtk_label_new("Core(s)                                                           ");
                        GtkWidget *cpu_cores_scale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 1, cpu_cores, 1);

			for(int i = 1; i <= cpu_cores; i++){
                                char *css_style_cpu_cores_config = "<span size='small'>";
                                char *i_string = malloc(sizeof(char) * (int)log10(i));
                                sprintf(i_string, "%d", i);
                                char *css_style_cpu_cores_suffix = "</span>";
                                char *css_style_cpu_cores_scale = malloc(strlen(css_style_cpu_cores_config) +
                                                                          strlen(css_style_cpu_cores_suffix) +
                                                                          strlen(i_string)  + 1);

                                strcpy(css_style_cpu_cores_scale, css_style_cpu_cores_config);
                                strcat(css_style_cpu_cores_scale, i_string);
                                strcat(css_style_cpu_cores_scale, css_style_cpu_cores_suffix);

                                gtk_scale_add_mark(GTK_SCALE(cpu_cores_scale), i, GTK_POS_BOTTOM, css_style_cpu_cores_scale);
                        }


      //gtk_scale_set_has_origin(GTK_SCALE(cpu_cores_scale), TRUE);
			                  gtk_range_set_round_digits(GTK_RANGE(cpu_cores_scale), 0);
                        gtk_range_set_increments(GTK_RANGE(cpu_cores_scale), 0, cpu_cores);
                        gtk_range_set_value(GTK_RANGE(cpu_cores_scale), cpu_cores - 1);
			                  selected_cpu_cores = cpu_cores - 1;

			g_signal_connect_swapped(cpu_cores_scale, "value_changed", G_CALLBACK(cpu_selected_cores_callback), cpu_cores_scale);

                        //cores box
                        gtk_box_append(GTK_BOX(box_cpu_cores), cpu_cores_label);
                        gtk_box_append(GTK_BOX(box_cpu_cores), cpu_cores_scale);



			GtkBox *box_cpu_threads = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 5));
                        GtkWidget *cpu_threads_label = gtk_label_new("Threads(s)                                                          ");
                        GtkWidget *cpu_threads_scale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 1, cpu_threads, 1);

			for(int i = 1; i <= cpu_threads; i++){
                                char *css_style_cpu_threads_config = "<span size='small'>";
                                char *i_string = malloc(sizeof(char) * (int)log10(i));
                                sprintf(i_string, "%d", i);
                                char *css_style_cpu_threads_suffix = "</span>";
                                char *css_style_cpu_threads_scale = malloc(strlen(css_style_cpu_threads_config) +
                                                                          strlen(css_style_cpu_threads_suffix) +
                                                                          strlen(i_string)  + 1);

                                strcpy(css_style_cpu_threads_scale, css_style_cpu_threads_config);
                                strcat(css_style_cpu_threads_scale, i_string);
                                strcat(css_style_cpu_threads_scale, css_style_cpu_threads_suffix);

                                gtk_scale_add_mark(GTK_SCALE(cpu_threads_scale), i, GTK_POS_BOTTOM, css_style_cpu_threads_scale);
                        }

                        gtk_scale_set_has_origin(GTK_SCALE(cpu_threads_scale), TRUE);
			gtk_range_set_round_digits(GTK_RANGE(cpu_threads_scale), 0);
												gtk_range_set_value(GTK_RANGE(cpu_threads_scale), cpu_threads);
			selected_cpu_threads = cpu_threads;

			g_signal_connect_swapped(cpu_threads_scale, "value_changed", G_CALLBACK(cpu_selected_threads_callback), cpu_threads_scale);

                        //socket box
                        gtk_box_append(GTK_BOX(box_cpu_threads), cpu_threads_label);
                        gtk_box_append(GTK_BOX(box_cpu_threads), cpu_threads_scale);





			//box_cpu
			gtk_box_append(GTK_BOX(box_cpu), cpu_TopLevelLabel);
			gtk_box_append(GTK_BOX(box_cpu), GTK_WIDGET(box_cpu_cores));
			gtk_box_append(GTK_BOX(box_cpu), GTK_WIDGET(box_cpu_threads));

			gtk_box_append(GTK_BOX(box_coloum_1), GTK_WIDGET(box_cpu));
		}


		//gpu
		{
			GtkBox *box_graphics = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 5));

			GtkWidget *graphics_label = gtk_label_new("Graphics                                                           ");

			char **graphics_selection_options;

			unsigned int n_graphics_selection_options = 3 + n_gpus;

			graphics_selection_options = malloc(sizeof(char*) * n_graphics_selection_options);

			graphics_selection_options[0] = malloc(sizeof(char) * strlen("Headless"));
			graphics_selection_options[0] = "Headless";

			graphics_selection_options[1] = malloc(sizeof(char) * strlen("Display Spice / Video QXL"));
                        graphics_selection_options[1] = "Display Spice / Video QXL";

			graphics_selection_options[2] = malloc(sizeof(char) * strlen("VNC"));
                        graphics_selection_options[2] = "VNC";

			for(int i = 0; i < n_gpus; i++){
				graphics_selection_options[i + 3] = malloc(sizeof(char) * strlen(gpu_names[i]));
				graphics_selection_options[i + 3] = gpu_names[i];
			}


			//const char **pass_input_const = (const char**)graphics_selection_options;
			//GtkWidget *drop_down_gpu_options = gtk_drop_down_new_from_strings(graphics_selection_options);
			GtkWidget *combo_box_graphics_options = combo_box_new_from_strings((const char**)graphics_selection_options, n_graphics_selection_options);

			g_signal_connect_swapped(combo_box_graphics_options, "changed", G_CALLBACK(changed_selected_graphics_callback), combo_box_graphics_options);



			gtk_box_append(GTK_BOX(box_graphics), GTK_WIDGET(graphics_label));
			gtk_box_append(GTK_BOX(box_graphics), GTK_WIDGET(combo_box_graphics_options));

			//gtk_grid_attach(GTK_GRID(install_settings_grid), GTK_WIDGET(box_graphics), 0, 1, 2, 1);
			gtk_box_append(GTK_BOX(box_coloum_1), GTK_WIDGET(box_graphics));
		}

		gtk_box_append(GTK_BOX(install_settings_box), GTK_WIDGET(box_coloum_1));



		GtkBox *box_coloum_2 = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 50));

		//storage size
		{
			GtkBox *box_storage = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 5));

			GtkWidget *storage_label = gtk_label_new("Storage                                                                        ");

			GtkWidget *storage_scale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, max_free_storage, 1);
			gtk_scale_add_mark(GTK_SCALE(storage_scale), 0, GTK_POS_BOTTOM, "<span size='small'>0GB</span>");

			{
                       	        char *css_style_storage_config = "<span size='small'>";
                                char *max_free_storage_string = malloc(sizeof(char) * (int)log10(max_free_storage));
                                sprintf(max_free_storage_string, "%d", (int)max_free_storage);
                                char *css_style_storage_suffix = "GB</span>";
                                char *css_style_storage_scale = malloc(strlen(css_style_storage_config) +
                                                                          strlen(css_style_storage_suffix) +
                                                                          strlen(max_free_storage_string)  + 1);

                                strcpy(css_style_storage_scale, css_style_storage_config);
                                strcat(css_style_storage_scale, max_free_storage_string);
                                strcat(css_style_storage_scale, css_style_storage_suffix);

                                gtk_scale_add_mark(GTK_SCALE(storage_scale), max_free_storage, GTK_POS_BOTTOM, css_style_storage_scale);

																if(max_free_storage > 50){
																	gtk_range_set_value(GTK_RANGE(storage_scale), 50);
	selected_storage = 50;
																}

			}

                        gtk_scale_set_has_origin(GTK_SCALE(storage_scale), TRUE);
			gtk_scale_set_draw_value(GTK_SCALE(storage_scale), TRUE);
			gtk_scale_set_value_pos(GTK_SCALE(storage_scale), GTK_POS_BOTTOM);

                        g_signal_connect_swapped(storage_scale, "value_changed", G_CALLBACK(storage_selected_callback), storage_scale);

			gtk_box_append(GTK_BOX(box_storage), GTK_WIDGET(storage_label));
			gtk_box_append(GTK_BOX(box_storage), GTK_WIDGET(storage_scale));
			gtk_box_append(GTK_BOX(box_coloum_2), GTK_WIDGET(box_storage));
		}

		//memory_allocation
		{
                        GtkBox *box_memory = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 5));

                        GtkWidget *memory_label = gtk_label_new("Memory Allocation                                                              ");

                        GtkWidget *memory_scale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, max_memory, 1);
                        gtk_scale_add_mark(GTK_SCALE(memory_scale), 0, GTK_POS_BOTTOM, "<span size='small'>0MB</span>");

                        {
                                char *css_style_memory_config = "<span size='small'>";
                                char *max_memory_string = malloc(sizeof(char) * (int)log10(max_memory));
                                sprintf(max_memory_string, "%d", (int)max_memory);
                                char *css_style_memory_suffix = "MB</span>";
                                char *css_style_memory_scale = malloc(strlen(css_style_memory_config) +
                                                                          strlen(css_style_memory_suffix) +
                                                                          strlen(max_memory_string)  + 1);

                                strcpy(css_style_memory_scale, css_style_memory_config);
                                strcat(css_style_memory_scale, max_memory_string);
                                strcat(css_style_memory_scale, css_style_memory_suffix);

                                gtk_scale_add_mark(GTK_SCALE(memory_scale), max_memory, GTK_POS_BOTTOM, css_style_memory_scale);

                        }

                        gtk_scale_set_has_origin(GTK_SCALE(memory_scale), TRUE);
												gtk_range_set_value(GTK_RANGE(memory_scale), max_memory / 2);
                        selected_memory = max_memory / 2;
												gtk_scale_set_draw_value(GTK_SCALE(memory_scale), TRUE);
                        gtk_scale_set_value_pos(GTK_SCALE(memory_scale), GTK_POS_BOTTOM);

												//PangoLayout *memory_layout = gtk_scale_get_layout(GTK_SCALE(memory_scale));
												//const gchar *const memory_markup = "<span color='red' size='small'>0MB</span>";
												//pango_layout_set_markup(memory_layout, memory_markup, (gint)strlen(memory_markup));

                        g_signal_connect_swapped(memory_scale, "value_changed", G_CALLBACK(memory_selected_callback), memory_scale);

                        gtk_box_append(GTK_BOX(box_memory), GTK_WIDGET(memory_label));
                        gtk_box_append(GTK_BOX(box_memory), GTK_WIDGET(memory_scale));
                        gtk_box_append(GTK_BOX(box_coloum_2), GTK_WIDGET(box_memory));

		}



		//bios
		{
			GtkBox *box_bios = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 5));

			GtkWidget *bios_label = gtk_label_new("BIOS								");

			const char* bios_selection_options[3] = {"UEFI", "UEFI with SecureBoot"};

			GtkWidget *combo_box_bios_options = combo_box_new_from_strings((const char**)bios_selection_options, 2);

			g_signal_connect_swapped(combo_box_bios_options, "changed", G_CALLBACK(changed_selected_bios_callback), combo_box_bios_options);


			gtk_box_append(GTK_BOX(box_bios), GTK_WIDGET(bios_label));
			gtk_box_append(GTK_BOX(box_bios), GTK_WIDGET(combo_box_bios_options));
			gtk_box_append(GTK_BOX(box_coloum_2), GTK_WIDGET(box_bios));
		}



		//TPM
		{
			GtkBox *box_tpm = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 5));

			GtkWidget *tpm_checkbutton = gtk_check_button_new();
			gtk_check_button_set_label(GTK_CHECK_BUTTON(tpm_checkbutton), "Emulated TPM(2.0)								");

			g_signal_connect(tpm_checkbutton, "toggled", G_CALLBACK(changed_selected_tpm_callback), tpm_checkbutton);

			gtk_box_append(GTK_BOX(box_tpm), GTK_WIDGET(tpm_checkbutton));
			gtk_box_append(GTK_BOX(box_coloum_2), GTK_WIDGET(box_tpm));
		}


		gtk_box_append(GTK_BOX(install_settings_box), GTK_WIDGET(box_coloum_2));

	}


	GtkBox *select_romfile_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 20));
        gtk_widget_set_halign(GTK_WIDGET(select_romfile_box), GTK_ALIGN_CENTER);
        gtk_widget_set_valign(GTK_WIDGET(select_romfile_box), GTK_ALIGN_CENTER);

        gtk_stack_add_named(GTK_STACK(stack), GTK_WIDGET(select_romfile_box), "select_romfile_box");

	{
	        romfile_icon = gtk_image_new_from_icon_name("document-new");
                romfile_filename_label = gtk_label_new("");
                gtk_image_set_icon_size(GTK_IMAGE(romfile_icon), GTK_ICON_SIZE_LARGE);

                //GtkWidget *file_label = gtk_label_new("");
		GtkWidget *explainer = gtk_label_new("To pass through some GPUs a patched romfile can be necessary");

                GtkButton *choose_romfile_button = GTK_BUTTON(gtk_button_new_with_label("Choose a romfile"));

                g_signal_connect_swapped(choose_romfile_button, "clicked", G_CALLBACK(choose_romfile_callback), window);
                //g_signal_connect_swapped(choose_iso_button, "clicked", G_CALLBACK(iso_file_icon_callback), iso_image);
                //g_signal_connect_swapped(choose_iso_button, "clicked", G_CALLBACK(iso_file_label_callback), file_label);

                gtk_box_append(GTK_BOX(select_romfile_box), GTK_WIDGET(romfile_icon));
                gtk_box_append(GTK_BOX(select_romfile_box), GTK_WIDGET(romfile_filename_label));
                gtk_box_append(GTK_BOX(select_romfile_box), GTK_WIDGET(choose_romfile_button));
		gtk_box_append(GTK_BOX(select_romfile_box), GTK_WIDGET(explainer));

	}


	GtkBox *bios_options_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 20));
        gtk_widget_set_halign(GTK_WIDGET(bios_options_box), GTK_ALIGN_CENTER);
        gtk_widget_set_valign(GTK_WIDGET(bios_options_box), GTK_ALIGN_CENTER);

        gtk_stack_add_named(GTK_STACK(stack), GTK_WIDGET(bios_options_box), "bios_options_box");

	{
		GtkWidget *general_bios_options_label = gtk_label_new("Please enable the following bios settings");
		GtkWidget *bios_setting_svm 	= gtk_label_new("SVM		enabled");
		GtkWidget *bios_setting_iommu	= gtk_label_new("IOMMU		enabled");
		GtkWidget *bios_setting_csm	= gtk_label_new("CMS support	enabled");

		gtk_box_append(GTK_BOX(bios_options_box), GTK_WIDGET(general_bios_options_label));
		gtk_box_append(GTK_BOX(bios_options_box), GTK_WIDGET(bios_setting_svm));
		gtk_box_append(GTK_BOX(bios_options_box), GTK_WIDGET(bios_setting_iommu));
		gtk_box_append(GTK_BOX(bios_options_box), GTK_WIDGET(bios_setting_csm));
	}



        GtkBox *install_vm_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 20));
        gtk_widget_set_halign(GTK_WIDGET(install_vm_box), GTK_ALIGN_CENTER);
        gtk_widget_set_valign(GTK_WIDGET(install_vm_box), GTK_ALIGN_CENTER);

        gtk_stack_add_named(GTK_STACK(stack), GTK_WIDGET(install_vm_box), "install_vm_box");

	{
		GtkWidget *install_button = gtk_button_new_with_label("Install VM");

		g_signal_connect_swapped(install_button, "clicked", G_CALLBACK(install_vm), NULL);

		gtk_box_append(GTK_BOX(install_vm_box), GTK_WIDGET(install_button));
	}




	gtk_window_set_titlebar(GTK_WINDOW(window), headerbar);

	if(!gtk_widget_get_visible(window)){
		gtk_widget_show(window);
	}
}




int main(int argc, char **argv){

	GtkApplication *app = gtk_application_new(APPLICATION_ID, G_APPLICATION_NON_UNIQUE);

	g_signal_connect(app, "startup", G_CALLBACK(app_startup), NULL);
	g_signal_connect(app, "activate", G_CALLBACK(app_activate), NULL);

  int status = g_application_run(G_APPLICATION(app),argc, argv);

	g_object_unref(app);

	return status;
}
