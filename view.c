/*-
 * Copyright (c) 2013-2014 Ilya Kaliman
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "vimol.h"

struct view {
	char *path;
	struct camera *camera;
	struct undo *undo;
};

static color_t
get_atom_color(const char *name)
{
	char tag[BUFSIZ], *ptr;

	strcpy(tag, "color.");
	strcpy(tag + 6, name);

	for (ptr = tag + 6; *ptr; ptr++)
		*ptr = (char)tolower(*ptr);

	if (settings_has_color(tag))
		return (settings_get_color(tag));

	return (settings_get_color("color.x"));
}

static void
render_atoms(struct view *view, cairo_t *cairo)
{
	struct sys *sys;
	struct sel *visible;
	color_t color;
	vec_t xyz;
	point_t pos;
	double size;
	int idx;

	size = settings_get_double("atom.size") / 2;
	sys = view_get_sys(view);
	visible = view_get_visible(view);

	sel_iter_start(visible);

	while (sel_iter_next(visible, &idx)) {
		xyz = sys_get_atom_xyz(sys, idx);
		pos = camera_transform(view->camera, xyz);

		if (cairo_in_clip(cairo, pos.x, pos.y)) {
			color = get_atom_color(sys_get_atom_name(sys, idx));
			cairo_set_source_rgb(cairo, color.r, color.g, color.b);
			cairo_arc(cairo, pos.x, pos.y, size, 0, 2 * PI);
			cairo_fill(cairo);
		}
	}
}

static void
render_bonds(struct view *view, cairo_t *cairo)
{
	struct sys *sys;
	struct graph *graph;
	struct edge *edge;
	struct sel *visible;
	color_t color;
	point_t p1, p2, p3, dp;
	double r, size;
	int i, j, k, type;

	size = settings_get_double("bond.size");
	sys = view_get_sys(view);
	graph = view_get_graph(view);
	visible = view_get_visible(view);

	cairo_set_line_width(cairo, size);
	sel_iter_start(visible);

	while (sel_iter_next(visible, &i)) {
		for (edge = graph_edges(graph, i); edge;
		     edge = graph_edge_next(edge)) {
			type = graph_edge_get_type(edge);
			j = graph_edge_j(edge);

			if (i > j || !sel_selected(visible, j))
				continue;

			p1 = camera_transform(view->camera,
			    sys_get_atom_xyz(sys, i));
			p3 = camera_transform(view->camera,
			    sys_get_atom_xyz(sys, j));

			if (!cairo_in_clip(cairo, p1.x, p1.y) &&
			    !cairo_in_clip(cairo, p3.x, p3.y))
				continue;

			dp.x = p1.y - p3.y;
			dp.y = p3.x - p1.x;

			r = sqrt(dp.x * dp.x + dp.y * dp.y);

			dp.x = 1.5 * size * dp.x / r;
			dp.y = 1.5 * size * dp.y / r;

			p1.x -= dp.x * (type - 1) / 2;
			p1.y -= dp.y * (type - 1) / 2;

			p3.x -= dp.x * (type - 1) / 2;
			p3.y -= dp.y * (type - 1) / 2;

			for (k = 0; k < type; k++) {
				p2.x = (p1.x + p3.x) / 2;
				p2.y = (p1.y + p3.y) / 2;

				color = get_atom_color(sys_get_atom_name(sys, i));
				cairo_set_source_rgb(cairo, color.r, color.g, color.b);
				cairo_move_to(cairo, p1.x, p1.y);
				cairo_line_to(cairo, p2.x, p2.y);
				cairo_stroke(cairo);

				color = get_atom_color(sys_get_atom_name(sys, j));
				cairo_set_source_rgb(cairo, color.r, color.g, color.b);
				cairo_move_to(cairo, p3.x, p3.y);
				cairo_line_to(cairo, p2.x, p2.y);
				cairo_stroke(cairo);

				p1.x += dp.x;
				p1.y += dp.y;

				p3.x += dp.x;
				p3.y += dp.y;
			}
		}
	}
}

static void
render_origin(struct view *view, cairo_t *cairo)
{
	color_t color;
	point_t p0, p1;
	mat_t e;
	double size, line;
	const char *font, *xyz[] = { "x", "y", "z" };
	int i;

	color = settings_get_color("origin.color");
	font = settings_get_string("origin.font");
	size = settings_get_double("origin.font.size");
	line = settings_get_double("origin.line.width");

	cairo_set_source_rgb(cairo, color.r, color.g, color.b);
	cairo_select_font_face(cairo, font, CAIRO_FONT_SLANT_NORMAL,
	    CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cairo, size);
	cairo_set_line_width(cairo, line);

	e = mat_identity();
	p0 = camera_transform(view->camera, vec_zero());

	for (i = 0; i < 3; i++) {
		p1 = camera_transform(view->camera, ((vec_t *)&e)[i]);
		cairo_move_to(cairo, p0.x, p0.y);
		cairo_line_to(cairo, p1.x, p1.y);
		cairo_move_to(cairo, p1.x + 3, p1.y + 3);
		cairo_show_text(cairo, xyz[i]);
	}

	cairo_stroke(cairo);
}

static void
render_ids(struct view *view, cairo_t *cairo)
{
	struct sys *sys;
	struct sel *visible;
	color_t color;
	point_t pos;
	double size;
	int idx;
	char buffer[32];
	const char *font;

	color = settings_get_color("id.color");
	font = settings_get_string("id.font");
	size = settings_get_double("id.font.size");

	cairo_set_source_rgb(cairo, color.r, color.g, color.b);
	cairo_select_font_face(cairo, font, CAIRO_FONT_SLANT_NORMAL,
	    CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cairo, size);

	sys = view_get_sys(view);
	visible = view_get_visible(view);

	sel_iter_start(visible);

	while (sel_iter_next(visible, &idx)) {
		pos = camera_transform(view->camera,
		    sys_get_atom_xyz(sys, idx));

		if (cairo_in_clip(cairo, pos.x, pos.y)) {
			snprintf(buffer, sizeof(buffer), "%d", idx + 1);
			cairo_move_to(cairo, pos.x + 6, pos.y + 10);
			cairo_show_text(cairo, buffer);
		}
	}
}

static void
render_names(struct view *view, cairo_t *cairo)
{
	struct sys *sys;
	struct sel *visible;
	color_t color;
	point_t pos;
	double size;
	int idx;
	const char *font, *name;

	color = settings_get_color("name.color");
	font = settings_get_string("name.font");
	size = settings_get_double("name.font.size");

	cairo_set_source_rgb(cairo, color.r, color.g, color.b);
	cairo_select_font_face(cairo, font, CAIRO_FONT_SLANT_NORMAL,
	    CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cairo, size);

	sys = view_get_sys(view);
	visible = view_get_visible(view);

	sel_iter_start(visible);

	while (sel_iter_next(visible, &idx)) {
		pos = camera_transform(view->camera,
		    sys_get_atom_xyz(sys, idx));

		if (cairo_in_clip(cairo, pos.x, pos.y)) {
			name = sys_get_atom_name(sys, idx);
			cairo_move_to(cairo, pos.x + 6, pos.y - 5);
			cairo_show_text(cairo, name);
		}
	}
}

static void
render_sel(struct view *view, cairo_t *cairo)
{
	struct sys *sys;
	struct sel *visible;
	struct sel *sel;
	color_t color;
	point_t pos;
	double size;
	int idx;

	color = settings_get_color("selection.color");
	size = settings_get_double("selection.size") / 2;

	cairo_set_source_rgb(cairo, color.r, color.g, color.b);

	sys = view_get_sys(view);
	visible = view_get_visible(view);
	sel = view_get_sel(view);

	sel_iter_start(sel);

	while (sel_iter_next(sel, &idx)) {
		if (!sel_selected(visible, idx))
			continue;

		pos = camera_transform(view->camera,
		    sys_get_atom_xyz(sys, idx));

		if (cairo_in_clip(cairo, pos.x, pos.y)) {
			cairo_arc(cairo, pos.x, pos.y, size, 0, 2 * PI);
			cairo_fill(cairo);
		}
	}
}

struct view *
view_create(const char *path)
{
	struct view *view;
	struct sys *sys;

	if ((sys = sys_create(path)) == NULL)
		return (NULL);

	view = calloc(1, sizeof(*view));
	view->camera = camera_create();
	view->undo = undo_create(sys, (void *(*)(void *))sys_copy,
	    (void (*)(void *))sys_free);

	view_set_path(view, path);
	view_reset(view);

	return (view);
}

void
view_free(struct view *view)
{
	camera_free(view->camera);
	undo_free(view->undo);
	free(view->path);
	free(view);
}

struct camera *
view_get_camera(struct view *view)
{
	return (view->camera);
}

struct sys *
view_get_sys(struct view *view)
{
	return (undo_get_data(view->undo));
}

struct graph *
view_get_graph(struct view *view)
{
	return (sys_get_graph(view_get_sys(view)));
}

struct sel *
view_get_sel(struct view *view)
{
	return (sys_get_sel(view_get_sys(view)));
}

struct sel *
view_get_visible(struct view *view)
{
	return (sys_get_visible(view_get_sys(view)));
}

const char *
view_get_path(struct view *view)
{
	return (view->path);
}

void
view_set_path(struct view *view, const char *path)
{
	view->path = xstrcpy(view->path, path);
}

int
view_is_empty(struct view *view)
{
	return (sys_get_atom_count(view_get_sys(view)) == 0);
}

int
view_is_modified(struct view *view)
{
	return (sys_is_modified(view_get_sys(view)));
}

int
view_undo(struct view *view)
{
	return (undo_undo(view->undo));
}

int
view_redo(struct view *view)
{
	return (undo_redo(view->undo));
}

void
view_snapshot(struct view *view)
{
	undo_snapshot(view->undo);
}

void
view_reset(struct view *view)
{
	struct sel *all;

	all = sel_create(sys_get_atom_count(view_get_sys(view)));

	sel_all(all);
	camera_reset(view->camera);
	view_center_sel(view, all);
	view_fit_sel(view, all);
	sel_free(all);
}

void
view_center_sel(struct view *view, struct sel *sel)
{
	vec_t xyz;

	if (sel_get_count(sel) == 0)
		return;

	xyz = sys_get_sel_center(view_get_sys(view), sel);
	camera_move_to(view->camera, xyz);
}

void
view_fit_sel(struct view *view, struct sel *sel)
{
	struct sys *sys;
	vec_t center, xyz;
	double radius;
	int idx;

	if (sel_get_count(sel) == 0)
		return;

	sys = view_get_sys(view);
	center = sys_get_sel_center(sys, sel);

	camera_set_radius(view->camera, 1.0);
	sel_iter_start(sel);

	while (sel_iter_next(sel, &idx)) {
		xyz = sys_get_atom_xyz(sys, idx);
		radius = vec_dist(&xyz, &center);

		if (radius > camera_get_radius(view->camera))
			camera_set_radius(view->camera, radius);
	}
}

void
view_render(struct view *view, cairo_t *cairo)
{
	color_t color;
	int width, height;
	double zoom;

	color = settings_get_color("bg.color");
	width = cairo_image_surface_get_width(cairo_get_target(cairo));
	height = cairo_image_surface_get_height(cairo_get_target(cairo));
	zoom = camera_get_zoom(view->camera, width, height);

	cairo_reset_clip(cairo);
	cairo_identity_matrix(cairo);
	cairo_set_source_rgb(cairo, color.r, color.g, color.b);
	cairo_paint(cairo);
	cairo_rectangle(cairo, 0, 0, width, height);
	cairo_clip(cairo);
	cairo_translate(cairo, width / 2, height / 2);
	cairo_scale(cairo, zoom, zoom);

	if (settings_get_bool("bond.visible"))
		render_bonds(view, cairo);

	if (settings_get_bool("atom.visible"))
		render_atoms(view, cairo);

	render_sel(view, cairo);

	if (settings_get_bool("origin.visible"))
		render_origin(view, cairo);

	if (settings_get_bool("name.visible"))
		render_names(view, cairo);

	if (settings_get_bool("id.visible"))
		render_ids(view, cairo);
}
