/*
	Copyright (C) 2021 Minho Jo <whitestone8214@gmail.com>
	
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* gcc $(pkg-config --cflags --libs gtk+-x11-2.0 filer) -fPIC main.c */


#include <filer.h>

#include <gtk/gtk.h>

#define WIDTH_PAGE 400
#define HEIGHT_PAGE 400
#define FONT_TO_USE "Source Han Sans KR 14"


typedef struct {
	GtkWidget widget;
	int edition;
	int index;
	byte *comment;
	
	int x;
	int y;
	int width;
	int height;
	
	int cursorAt;
	char cursorDirection;
	char modeSelect;
	int selectFrom;
	int selectTo;
	int cursorDistance;
	
	byte *data;
} GtkTextBox;
typedef struct {GtkWidgetClass parent;} GtkTextBoxClass;
typedef struct {
	char *letter;
	int x;
	int y;
	int width;
	int height;
	char selected;
} Piece;


unsigned long _idGtkTextBox;

byte *_valueStringA, *_valueStringB, *_valueStringC;
cell *_listLetters;
cell *_listPieces;
cell *_listLinePositions;
int _valueMaxWidth;
GtkWidget *_widgetLabelDummy;
PangoContext *_whatever;

GtkWidget *_windowMain;
GtkWidget *_boxMain;
GtkWidget *_boxABC;
GtkWidget *_routeA;
GtkWidget *_routeB;
GtkWidget *_routeC;
GtkWidget *_boxR1;
GtkWidget *_routeCopy;


int main(int nOptions, char **options, char **conditions);
void when_widget_deleted(GtkWidget *this, void *data);
char page_sizing(GtkBox *this, GtkRequisition *sizing);
char page_placing(GtkBox *this, GtkAllocation *placing);

cell *string_to_cells(char *string);
cell *letters_to_pieces(cell *letters);

unsigned long gtk_textbox_id();
void gtk_textbox_triggers(GtkTextBoxClass *class);
void gtk_textbox_properties(GtkTextBox *this);
void gtk_textbox_dispose(GtkWidget *this);
void gtk_textbox_show(GtkWidget *this);
void gtk_textbox_sizing(GtkWidget *this, GtkRequisition *sizing);
void gtk_textbox_placing(GtkWidget *this, GtkAllocation *placing);
gboolean gtk_textbox_looks(GtkWidget *this, GdkEventExpose *event);
gboolean gtk_textbox_about_mouse_cursor(GtkWidget *this, GdkEventMotion *event, void *parcel);
gboolean gtk_textbox_about_mouse_button(GtkWidget *this, GdkEventButton *event, void *parcel);
void gtk_textbox_to_clipboard(GtkWidget *this, void *parcel);
void gtk_textbox_clear(GtkWidget *this);
void gtk_textbox_read_a(GtkWidget *this, void *parcel);
void gtk_textbox_read_b(GtkWidget *this, void *parcel);
void gtk_textbox_read_c(GtkWidget *this, void *parcel);

char gdk_draw_point_iiji(GdkDrawable *canvas, guint32 color, int x, int y);
char gdk_draw_line_iiji(GdkDrawable *canvas, guint32 color, int xFrom, int yFrom, int xTo, int yTo);
char gdk_draw_rectangle_iiji(GdkDrawable *canvas, guint32 stroke, guint32 fill, gboolean filled, int x, int y, int width, int height);
char gdk_draw_layout_iiji(GdkDrawable *canvas, PangoContext *pango, int x, int y, int width, int height, char *string, char *font, guint32 stroke, guint32 fill);
void gdk_calculate_layout_size(char *string, char *font, int *width, int *height);


int main(int nOptions, char **options, char **conditions) {
	gtk_init(&nOptions, &options);
	_idGtkTextBox = gtk_textbox_id();
	_valueStringA = "다람쥐\n헌 쳇바퀴에 타고파";
	_valueStringB = "더 퀵 브라운 폭스\n점프스 오버\n레이지 도그";
	_valueStringC = "로렘 입숨 돌로르 시트 아메트 콘세크테투르 아디피시싱 엘리트";
	_valueMaxWidth = 360;
	_listLinePositions = NULL;
	_widgetLabelDummy = gtk_label_new("");
	
	// Main window
	_windowMain = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_resize((GtkWindow *)_windowMain, WIDTH_PAGE, HEIGHT_PAGE);
	g_signal_connect(_windowMain, "delete_event", (GCallback)when_widget_deleted, NULL);
	gtk_window_set_position((GtkWindow *)_windowMain, GTK_WIN_POS_CENTER);
	
	_boxMain = gtk_vbox_new(FALSE, 0);
	gtk_container_add((GtkContainer *)_windowMain, _boxMain);
	
	// Main box
	_boxR1 = g_object_new(_idGtkTextBox, NULL);
	GtkTextBox *_boxR1A = (GtkTextBox *)_boxR1;
	_boxR1A->width = WIDTH_PAGE;
	_boxR1A->height = HEIGHT_PAGE;
	_boxR1A->cursorAt = 0;
	_boxR1A->cursorDirection = 0;
	_boxR1A->modeSelect = 0;
	_boxR1A->selectFrom = 0;
	_boxR1A->selectTo = 0;
	_boxR1A->cursorDistance = 0;
	//_boxR1A->data = (byte *)strdup(_valueStringA);
	_boxR1A->data = NULL;
	g_signal_connect(_boxR1, "motion_notify_event", (GCallback)gtk_textbox_about_mouse_cursor, NULL);
	g_signal_connect(_boxR1, "button_press_event", (GCallback)gtk_textbox_about_mouse_button, NULL);
	g_signal_connect(_boxR1, "button_release_event", (GCallback)gtk_textbox_about_mouse_button, NULL);
	//gtk_container_add((GtkContainer *)_windowMain, _boxR1);
	gtk_box_pack_start((GtkBox *)_boxMain, _boxR1, FALSE, FALSE, 0);
	
	_boxABC = gtk_hbox_new(FALSE, 0);
	gtk_container_add((GtkContainer *)_boxMain, _boxABC);
	
	_routeA = gtk_button_new_with_label("A");
	g_signal_connect(_routeA, "clicked", (GCallback)gtk_textbox_read_a, NULL);
	gtk_box_pack_start((GtkBox *)_boxABC, _routeA, TRUE, TRUE, 0);
	
	_routeB = gtk_button_new_with_label("B");
	g_signal_connect(_routeB, "clicked", (GCallback)gtk_textbox_read_b, NULL);
	gtk_box_pack_start((GtkBox *)_boxABC, _routeB, TRUE, TRUE, 0);
	
	_routeC = gtk_button_new_with_label("C");
	g_signal_connect(_routeC, "clicked", (GCallback)gtk_textbox_read_c, NULL);
	gtk_box_pack_start((GtkBox *)_boxABC, _routeC, TRUE, TRUE, 0);
	
	_routeCopy = gtk_button_new_with_label("Copy selection to clipboard");
	g_signal_connect(_routeCopy, "clicked", (GCallback)gtk_textbox_to_clipboard, NULL);
	gtk_box_pack_start((GtkBox *)_boxMain, _routeCopy, TRUE, TRUE, 0);
	
	_whatever = pango_font_map_create_context(pango_cairo_font_map_get_default());
			
	// Go!
	warp_main:
	gtk_textbox_read_a(NULL, NULL);
	gtk_widget_show_all(_windowMain);
	gtk_main();
	
	return 0;
}
void when_widget_deleted(GtkWidget *this, void *data) {
	gtk_textbox_clear(_boxR1);
	if (_whatever != NULL) {g_object_unref(_whatever); _whatever = NULL;}
	gtk_main_quit();
}
char page_sizing(GtkBox *this, GtkRequisition *sizing) {
	if (this == NULL) return 0;
	
	GtkWidget *_this = (GtkWidget *)this;
	GtkWidget *_window = gtk_widget_get_toplevel(_this);
	if (_window != NULL) gtk_window_get_size((GtkWindow *)_window, &sizing->width, &sizing->height);
	
	return 1;
}
char page_placing(GtkBox *this, GtkAllocation *placing) {
	if (this == NULL) return 0;
	
	GtkWidget *_this = (GtkWidget *)this;
	GtkWidget *_window = gtk_widget_get_toplevel(_this);
	if (_window != NULL) gtk_window_get_size((GtkWindow *)_window, &placing->width, &placing->height);
	placing->x = 0;
	placing->y = 0;
	
	return 1;
}

cell *string_to_cells(char *string) {
	if (string == NULL) return NULL;
	
	cell *_cell = cell_create(NULL);
	cell *_cell1 = _cell;
	int _nthLetter;
	for (_nthLetter = 0; _nthLetter < strlen(string); _nthLetter++) {
		_cell1->data = (void *)string[_nthLetter];
		cell_insert(_cell1, -1, NULL);
		_cell1 = _cell1->next;
	}
	
	return _cell;
}
cell *letters_to_pieces(cell *letters) {
	if (letters == NULL) return NULL;
	if (_listLinePositions != NULL) cell_delete_all(_listLinePositions, 0);
	
	_listLinePositions = cell_create((void *)-1);
	cell *_cell = cell_create(NULL);
	cell *_cell1 = _cell;
	int _x = 0;
	int _y = 0;
	int _nthLetter;
	for (_nthLetter = 0; _nthLetter < cell_length(_listLetters); _nthLetter++) {
		// Record the start of line
		if (_x == 0) {
			_listLinePositions->data = (void *)_nthLetter;
			cell_insert(_listLinePositions, -1, (void *)-1);
			_listLinePositions = _listLinePositions->next;
		}
		
		// Table
		Piece *_piece = (Piece *)malloc(sizeof(Piece) + 1);
		if (_piece == NULL) break;
		
		byte _byte = (byte)cell_nth(_listLetters, _nthLetter)->data;
		char *_letter1 = NULL;
		if (_byte == 0) break; // \0 (0x00) = The End
		else if ((_byte == 0x0a) || (_byte == 0x13)) { // LF (0x0a), CR (0x13) = Go to next line
			byte _letter[2];
			_letter[0] = _byte;
			_letter[1] = '\0';
			
			_piece->letter = strdup(&_letter[0]);
			_piece->x = _x;
			_piece->y = _y;
			_piece->width = 0;
			gdk_calculate_layout_size("?", FONT_TO_USE, NULL, &_piece->height);
			
			_x = 0;
			_y += _piece->height;
		}
		else if (_byte < 0x80) { // Single-byte letter (0x00 ~ 0x7f)
			byte _letter[2];
			_letter[0] = _byte;
			_letter[1] = '\0';
			
			_piece->letter = strdup(&_letter[0]);
			_piece->x = _x;
			_piece->y = _y;
			gdk_calculate_layout_size(&_letter[0], FONT_TO_USE, &_piece->width, &_piece->height);
		}
		else if ((_byte >= 0xc2) && (_byte <= 0xf4)) { // Start of multi-byte letter (0xc2 ~ 0xf4)
			int _nthLetterTo = _nthLetter + 1;
			while ((_nthLetterTo <= cell_length(_listLetters) - 1) && ((byte)cell_nth(_listLetters, _nthLetterTo)->data >= 0x80) && ((byte)cell_nth(_listLetters, _nthLetterTo)->data < 0xc0)) _nthLetterTo++;
			
			int _nBytes = _nthLetterTo - _nthLetter;
			byte _letter[_nBytes + 1];
			memset(&_letter[0], 0, _nBytes + 1);
			int _nthLetterIn;
			for (_nthLetterIn = _nthLetter; _nthLetterIn < _nthLetterTo; _nthLetterIn++) _letter[_nthLetterIn - _nthLetter] = (byte)cell_nth(_listLetters, _nthLetterIn)->data;
			
			_piece->letter = strdup(&_letter[0]);
			_piece->x = _x;
			_piece->y = _y;
			gdk_calculate_layout_size(&_letter[0], FONT_TO_USE, &_piece->width, &_piece->height);
			_nthLetter = _nthLetterTo - 1;
		}
		
		// Enlist	
		_cell1->data = _piece;
		cell_insert(_cell1, -1, NULL);
		_cell1 = _cell1->next;
		
		if (_x >= _valueMaxWidth) { // Go to next line if the line is full
			_x = 0;
			_y += _piece->height;
		}
		else { // ...or just go right
			_x += _piece->width;
		}
	}
	
	_listLinePositions = cell_nth(_listLinePositions, 0);	
	return _cell;
}

unsigned long gtk_textbox_id() {
	const GtkTypeInfo _tree = {
		"GtkTextBox",
		sizeof(GtkTextBox),
		sizeof(GtkTextBoxClass),
		(GtkClassInitFunc)gtk_textbox_triggers,
		(GtkObjectInitFunc)gtk_textbox_properties,
		NULL,
		NULL,
		(GtkClassInitFunc)NULL
	};
	
	return gtk_type_unique(gtk_widget_get_type(), &_tree);
}
void gtk_textbox_triggers(GtkTextBoxClass *class) {
	GtkWidgetClass *_classAsWidget = (GtkWidgetClass *)class;
	_classAsWidget->realize = gtk_textbox_show;
	_classAsWidget->size_request = gtk_textbox_sizing;
	_classAsWidget->size_allocate = gtk_textbox_placing;
	_classAsWidget->expose_event = gtk_textbox_looks;
	_classAsWidget->unrealize = gtk_textbox_dispose;
}
void gtk_textbox_properties(GtkTextBox *this) {
	this->edition = 0;
	this->index = 0;
	this->comment = NULL;
	
	this->x = 0;
	this->y = 0;
	this->width = 0;
	this->height = 0;
	this->data = NULL;
}
void gtk_textbox_dispose(GtkWidget *this) {
	gdk_window_set_user_data(gtk_widget_get_window(this), NULL);
	GtkTextBox *_this = (GtkTextBox *)this;
	
	if (_this->comment != NULL) {free(_this->comment); _this->comment = NULL;}
	if (_this->data != NULL) {free(_this->data); _this->data = NULL;}
	
	GtkTextBoxClass *_class = (GtkTextBoxClass *)gtk_type_class(gtk_widget_get_type());
	if (GTK_OBJECT_CLASS(_class)->destroy != NULL) (* GTK_OBJECT_CLASS(_class)->destroy)((GtkObject *)this);
}
void gtk_textbox_show(GtkWidget *this) {
	gtk_widget_set_realized(this, TRUE);
	GtkAllocation _placing; gtk_widget_get_allocation(this, &_placing);
	
	GtkTextBox *_this = (GtkTextBox *)this;
	GdkWindowAttr _settings;
	_settings.x = _this->x;
	_settings.y = _this->y;
	_settings.width = _this->width;
	_settings.height = _this->height;
	_settings.window_type = GDK_WINDOW_CHILD;
	_settings.wclass = GDK_INPUT_OUTPUT;
	_settings.event_mask = GDK_ALL_EVENTS_MASK;
	
	GdkWindow *_canvas = gdk_window_new(gtk_widget_get_parent_window(this), &_settings, GDK_WA_X | GDK_WA_Y);
	gdk_window_set_user_data(_canvas, this);
	gtk_widget_set_window(this, _canvas);
}
void gtk_textbox_sizing(GtkWidget *this, GtkRequisition *sizing) {
	GtkTextBox *_this = (GtkTextBox *)this;
	sizing->width = _this->width;
	sizing->height = _this->height;
}
void gtk_textbox_placing(GtkWidget *this, GtkAllocation *placing) {
	GtkTextBox *_this = (GtkTextBox *)this;
	placing->x = _this->x;
	placing->y = _this->y;
	placing->width = _this->width;
	placing->height = _this->height;
}
gboolean gtk_textbox_looks(GtkWidget *this, GdkEventExpose *event) {
	// Prepare
	if (this == NULL) return FALSE;
	GtkTextBox *_this = (GtkTextBox *)this;
	GdkDrawable *_canvas = (GdkDrawable *)gtk_widget_get_window(this);
	if (_canvas == NULL) return FALSE;
	
	// Background
	gdk_draw_rectangle_iiji(_canvas, 0xffffffff, 0xffffffff, TRUE, 0, 0, _this->width, _this->height);
	
	// No text?
	if (_listLetters == NULL) return TRUE;
	if (_listPieces == NULL) return TRUE;
	
	int _nthPiece;
	for (_nthPiece = 0; _nthPiece < cell_length(_listPieces); _nthPiece++) {
		// Sanity check and preparation
		Piece *_piece = (Piece *)cell_nth(_listPieces, _nthPiece)->data;
		if (_piece == NULL) break;
		if (_piece->letter == NULL) break;
		char *_letter = _piece->letter;
		
		// 0x00 = The end
		if (_letter[0] == 0) break;
		
		// Draw the letter
		if (_piece->selected == 1) gdk_draw_rectangle_iiji(_canvas, 0xffcfcf30, 0xffcfcf30, TRUE, _piece->x, _piece->y, _piece->width, _piece->height);
		else gdk_draw_rectangle_iiji(_canvas, 0xffffffff, 0xffffffff, TRUE, _piece->x, _piece->y, _piece->width, _piece->height);
		if (_nthPiece == _this->cursorAt) {
			gdk_draw_rectangle_iiji(_canvas, 0xffff7f00, 0xffff7f00, TRUE,
				(_this->cursorDirection == 1) ? ((_piece->x + _piece->width) - (_piece->width / 10)) : _piece->x, _piece->y, _piece->width / 10, _piece->height
			);
		}
		gdk_draw_layout_iiji(_canvas, _whatever, _piece->x, _piece->y, 1, 1, _piece->letter, FONT_TO_USE, 0xff303030, 0xff303030);
	}
	
	return TRUE;
}
gboolean gtk_textbox_about_mouse_cursor(GtkWidget *this, GdkEventMotion *event, void *parcel) {
	// Ready
	GtkTextBox *_this = (GtkTextBox *)this;
	GdkDrawable *_canvas = (GdkDrawable *)gtk_widget_get_window(this);
	int _x = (int)event->x;
	int _y = (int)event->y;
	//printf("STATE %d\n", event->state);
	
	// Initialize
	GdkRectangle _areaEntire; _areaEntire.x = 0; _areaEntire.y = 0; _areaEntire.width = WIDTH_PAGE; _areaEntire.height = HEIGHT_PAGE;
	gdk_window_invalidate_rect(_canvas, &_areaEntire, TRUE);
	
	int _nthPiece;
	for (_nthPiece = 0; _nthPiece < cell_length(_listPieces); _nthPiece++) {
		// Sanity check
		Piece *_piece = (Piece *)cell_nth(_listPieces, _nthPiece)->data;
		if (_piece == NULL) continue;
		
		if (_this->modeSelect == 1) {
			_this->cursorDistance += 1;
			
			if ((_x >= _piece->x) && (_x < _piece->x + _piece->width) && (_y >= _piece->y) && (_y < _piece->y + _piece->height)) {
				_this->selectTo = _nthPiece;
				int _nthPiece1;
				for (_nthPiece1 = 0; _nthPiece1 < cell_length(_listPieces); _nthPiece1++) {
					if (cell_nth(_listPieces, _nthPiece1) == NULL) break;
					if (cell_nth(_listPieces, _nthPiece1)->data == NULL) break;
					
					if (_this->selectFrom == _this->selectTo) ((Piece *)cell_nth(_listPieces, _nthPiece1)->data)->selected = (_nthPiece1 == _this->selectFrom) ? 1 : 0;
					else if (_this->selectFrom > _this->selectTo) {
						((Piece *)cell_nth(_listPieces, _nthPiece1)->data)->selected = ((_nthPiece1 >= _this->selectTo) && (_nthPiece1 <= _this->selectFrom)) ? 1 : 0;
					}
					else {
						((Piece *)cell_nth(_listPieces, _nthPiece1)->data)->selected = ((_nthPiece1 >= _this->selectFrom) && (_nthPiece1 <= _this->selectTo)) ? 1 : 0;
					}
				}
				
				//printf("SELECT %d -> %d\n", _this->selectFrom, _this->selectTo);
			}
		}
	}
	
	return FALSE;
}
gboolean gtk_textbox_about_mouse_button(GtkWidget *this, GdkEventButton *event, void *parcel) {
	GtkTextBox *_this = (GtkTextBox *)this;
	GdkDrawable *_canvas = (GdkDrawable *)gtk_widget_get_window(this);
	int _x = (int)event->x;
	int _y = (int)event->y;
	
	//printf("BUTTON!\n");
	int _nthPiece;
	_this->modeSelect = ((event->button == 1) && (event->state == 0)) ? 1 : 0;
	if (_this->modeSelect == 1) {
		_this->cursorDistance = 0;
		
		for (_nthPiece = 0; _nthPiece < cell_length(_listPieces); _nthPiece++) {
			// Sanity check
			Piece *_piece = (Piece *)cell_nth(_listPieces, _nthPiece)->data;
			if (_piece == NULL) continue;
			
			// Selected letter
			if ((_x >= _piece->x) && (_x < _piece->x + _piece->width) && (_y >= _piece->y) && (_y < _piece->y + _piece->height)) {
				_this->selectFrom = _nthPiece;
				_this->selectTo = _nthPiece;
			}
		}
	}
	else {
		if (_this->cursorDistance < 1) {
			for (_nthPiece = 0; _nthPiece < cell_length(_listPieces); _nthPiece++) {
				// Sanity check
				Piece *_piece = (Piece *)cell_nth(_listPieces, _nthPiece)->data;
				if (_piece == NULL) continue;
				_piece->selected = 0;
			}
			
			GdkRectangle _areaEntire; _areaEntire.x = 0; _areaEntire.y = 0; _areaEntire.width = WIDTH_PAGE; _areaEntire.height = HEIGHT_PAGE;
			gdk_window_invalidate_rect(_canvas, &_areaEntire, TRUE);
		}
		
		for (_nthPiece = 0; _nthPiece < cell_length(_listPieces); _nthPiece++) {
			// Sanity check
			Piece *_piece = (Piece *)cell_nth(_listPieces, _nthPiece)->data;
			if (_piece == NULL) continue;
			
			// Selected letter
			if ((_x >= _piece->x) && (_x < _piece->x + _piece->width) && (_y >= _piece->y) && (_y < _piece->y + _piece->height)) {
				_this->cursorAt = _nthPiece;
				_this->cursorDirection = (_x - _piece->x < _piece->width / 2) ? 0 : 1;
				break;
			}
		}
	}
	
	return FALSE;
}
void gtk_textbox_to_clipboard(GtkWidget *this, void *parcel) {
	char *_string = NULL;
	int _nthPiece;
	for (_nthPiece = 0; _nthPiece < cell_length(_listPieces); _nthPiece++) {
		// Sanity check
		Piece *_piece = (Piece *)cell_nth(_listPieces, _nthPiece)->data;
		if (_piece == NULL) continue;
		if (_piece->selected != 1) continue;
		if (_piece->letter == NULL) continue;
		
		// Connect the letters
		if (_string == NULL) _string = strdup(_piece->letter);
		else {
			char *_string1 = byter_connect_strings(2, _string, _piece->letter);
			free(_string);
			_string = _string1;
		}
	}
	
	// Write to clipboard
	if (_string != NULL) {
		GtkClipboard *_clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
		if (_clipboard != NULL) gtk_clipboard_set_text(_clipboard, _string, strlen(_string));
		free(_string);
	}
}
void gtk_textbox_clear(GtkWidget *this) {
	printf("A\n");
	// Clear widget
	GtkTextBox *_this = (GtkTextBox *)this;
	_this->cursorAt = 0;
	_this->cursorDirection = 0;
	_this->modeSelect = 0;
	_this->selectFrom = 0;
	_this->selectTo = 0;
	_this->cursorDistance = 0;
	if (_this->comment != NULL) {free(_this->comment); _this->comment = NULL;}
	if (_this->data != NULL) {free(_this->data); _this->data = NULL;}
	printf("B\n");
	
	// Clear actual data
	if (_listLinePositions != NULL) {cell_delete_all(_listLinePositions, 0); _listLinePositions = NULL;}
	if (_listPieces != NULL) {
		int _nthPiece;
		for (_nthPiece = cell_length(_listPieces) - 1; _nthPiece >= 0; _nthPiece--) {
			Piece *_piece = (Piece *)cell_nth(_listPieces, _nthPiece)->data;
			if (_piece == NULL) continue;
			
			if (_piece->letter != NULL) free(_piece->letter);
			cell_delete(cell_nth(_listPieces, _nthPiece), 1);
		}
		_listPieces = NULL;
	}
	if (_listLetters != NULL) {cell_delete_all(_listLetters, 0); _listLetters = NULL;}
	printf("Z\n");
}
void gtk_textbox_read_a(GtkWidget *this, void *parcel) {
	gtk_textbox_clear(_boxR1);
	
	((GtkTextBox *)_boxR1)->data = strdup(_valueStringA);
	_listLetters = string_to_cells(_valueStringA);
	_listPieces = letters_to_pieces(_listLetters);
	
	gtk_widget_queue_draw(_windowMain);
}
void gtk_textbox_read_b(GtkWidget *this, void *parcel) {
	gtk_textbox_clear(_boxR1);
	((GtkTextBox *)_boxR1)->data = "?";
	
	((GtkTextBox *)_boxR1)->data = strdup(_valueStringB);
	_listLetters = string_to_cells(_valueStringB);
	_listPieces = letters_to_pieces(_listLetters);
	
	gtk_widget_queue_draw(_windowMain);
}
void gtk_textbox_read_c(GtkWidget *this, void *parcel) {
	gtk_textbox_clear(_boxR1);
	
	((GtkTextBox *)_boxR1)->data = strdup(_valueStringC);
	_listLetters = string_to_cells(_valueStringC);
	_listPieces = letters_to_pieces(_listLetters);
	
	gtk_widget_queue_draw(_windowMain);
}

char gdk_draw_point_iiji(GdkDrawable *canvas, guint32 color, int x, int y) {
	// Sanity check
	if (canvas == NULL) return 0;
	
	// Brush
	GdkGC *_brush = gdk_gc_new(canvas);
	GdkColor _color; _color.pixel = color;
	gdk_gc_set_foreground(_brush, &_color);
	gdk_gc_set_background(_brush, &_color);
	
	// Draw
	gdk_draw_point(canvas, _brush, x, y);
	
	// Dispose
	g_object_unref(_brush);
}
char gdk_draw_line_iiji(GdkDrawable *canvas, guint32 color, int xFrom, int yFrom, int xTo, int yTo) {
	// Sanity check
	if (canvas == NULL) return 0;
	
	// Brush
	GdkGC *_brush = gdk_gc_new(canvas);
	GdkColor _color; _color.pixel = color;
	gdk_gc_set_foreground(_brush, &_color);
	gdk_gc_set_background(_brush, &_color);
	
	// Draw
	gdk_draw_line(canvas, _brush, xFrom, yFrom, xTo, yTo);
	
	// Dispose
	g_object_unref(_brush);
}
char gdk_draw_rectangle_iiji(GdkDrawable *canvas, guint32 stroke, guint32 fill, gboolean filled, int x, int y, int width, int height) {
	// Sanity check
	if (canvas == NULL) return 0;
	
	// Brush
	GdkGC *_brush = gdk_gc_new(canvas);
	GdkColor _colorStroke; _colorStroke.pixel = stroke;
	GdkColor _colorFill; _colorFill.pixel = fill;
	gdk_gc_set_foreground(_brush, &_colorStroke);
	gdk_gc_set_background(_brush, &_colorFill);
	
	// Draw
	gdk_draw_rectangle(canvas, _brush, filled, x, y, width, height);
	
	// Dispose
	g_object_unref(_brush);
}
char gdk_draw_layout_iiji(GdkDrawable *canvas, PangoContext *pango, int x, int y, int width, int height, char *string, char *font, guint32 stroke, guint32 fill) {
	// Sanity check
	if (canvas == NULL) return 0;
	if (pango == NULL) return 0;
	if (width < 1) return 0;
	if (height < 1) return 0;
	if (string == NULL) return 0;
	if (strlen(string) < 1) return 0;
	
	// Layout
	PangoLayout *_layout = pango_layout_new(pango);
	if (_layout == NULL) return 0;
	pango_layout_set_text(_layout, string, -1);
	
	// Font
	PangoFontDescription *_font = pango_font_description_from_string(font);
	if (_font == NULL) {g_object_unref(_layout); return 0;}
	pango_layout_set_font_description(_layout, _font);
	
	// Background
	GdkGC *_brushBackground = gdk_gc_new(canvas);
	if (_brushBackground == NULL) {pango_font_description_free(_font); g_object_unref(_layout); return 0;}
	GdkColor _colorFill; _colorFill.pixel = fill;
	gdk_gc_set_foreground(_brushBackground, &_colorFill);
	gdk_gc_set_background(_brushBackground, &_colorFill);
	gdk_draw_rectangle(canvas, _brushBackground, TRUE, 0, 0, width, height);
	g_object_unref(_brushBackground);
	
	// String
	GdkGC *_brushString = gdk_gc_new(canvas);
	if (_brushString == NULL) {pango_font_description_free(_font); g_object_unref(_layout); return 0;}
	GdkColor _colorStroke; _colorStroke.pixel = stroke;
	gdk_gc_set_foreground(_brushString, &_colorStroke);
	gdk_gc_set_background(_brushString, &_colorStroke);
	gdk_draw_layout(canvas, _brushString, x, y, _layout);
	g_object_unref(_brushString);
	
	// Dispose
	//g_object_unref(_brush);
	pango_font_description_free(_font);
	g_object_unref(_layout);
}
void gdk_calculate_layout_size(char *string, char *font, int *width, int *height) {
	// Sanity check
	if (string == NULL) return;
	if ((width == NULL) && (height == NULL)) return;
	
	PangoContext *_context = pango_font_map_create_context(pango_cairo_font_map_get_default());
	if (_context == NULL) return;
	
	// Layout
	PangoLayout *_layout = pango_layout_new(_context);
	if (_layout == NULL) {g_object_unref(_context); return;}
	pango_layout_set_text(_layout, string, -1);
	
	// Font
	PangoFontDescription *_font = NULL;
	if (font != NULL) {
		_font = pango_font_description_from_string(font);
		if (_font == NULL) {g_object_unref(_context); g_object_unref(_layout); return;}
		pango_layout_set_font_description(_layout, _font);
	}
	
	// Calculate
	pango_layout_get_pixel_size(_layout, width, height);
	
	// Dispose
	if (_font != NULL) pango_font_description_free(_font);
	g_object_unref(_layout);
	g_object_unref(_context);
}
