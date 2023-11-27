// Copyright (c) 2023 Aaron Ayub
// SPDX-License-Identifier: GPL-3.0-only

#include <gtk/gtk.h>

#include "di-file-cell.h"

struct _DiFileCell {
	GtkGrid parent;

	GtkWidget *name;
	GtkWidget *size;
	GtkImage *image;
};

G_DEFINE_TYPE (DiFileCell, di_file_cell, GTK_TYPE_GRID)

static void di_file_cell_init (DiFileCell *cell) {
	gtk_widget_init_template (GTK_WIDGET (cell));
}

static void di_file_cell_class_init (DiFileCellClass *class) {
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (class);
	gtk_widget_class_set_template_from_resource (widget_class,
			"/com/github/AaronAyub/dropit/file-cell.ui");
	gtk_widget_class_bind_template_child (widget_class, DiFileCell, name);
	gtk_widget_class_bind_template_child (widget_class, DiFileCell, size);
	gtk_widget_class_bind_template_child (widget_class, DiFileCell, image);
}

DiFileCell *di_file_cell_new (void) {
	return g_object_new (DI_FILE_CELL_TYPE, NULL);
}

void di_file_cell_load (DiFileCell *cell, GFile *file) {
	// Set up image and labels
	GFileInfo *fileInfo = g_file_query_info (file, "standard::name,standard::size,standard::icon", G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, NULL, NULL);
	GIcon *icon = g_file_info_get_icon (fileInfo);
	const char *name = g_file_info_get_name (fileInfo);
	goffset size = g_file_info_get_size (fileInfo);
	gchar *sizeText = g_strdup_printf("%ldB", size);
	gtk_image_set_from_gicon (cell->image, icon);
	gtk_label_set_label (GTK_LABEL (cell->name), name);
	gtk_label_set_label (GTK_LABEL (cell->size), sizeText);

	g_free (sizeText);

	// Set the drag source
	GtkDragSource *dsource;
	GdkContentProvider *contentProvider;

	dsource = gtk_drag_source_new ();
	GdkFileList* fileList = gdk_file_list_new_from_array (&file, 1);
	contentProvider = gdk_content_provider_new_typed (GDK_TYPE_FILE_LIST, fileList);

	gtk_drag_source_set_content (dsource, contentProvider);
	g_object_unref (contentProvider);
	gtk_widget_add_controller (GTK_WIDGET (cell), GTK_EVENT_CONTROLLER (dsource));
}
