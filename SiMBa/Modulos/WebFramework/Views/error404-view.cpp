#include "views.h"
#include "app.h"

static Content htmlBody;

ErrorView::ErrorView(App * app, Simba * simba) : View( app, simba ) {

    htmlBody.Init("  <div class=\"container py-5 text-center\">"
"    <h1 class=\"display-1 fw-bold text-danger\">404</h1>"
"    <h2 class=\"mb-4\">Página no encontrada</h2>"
"    <p class=\"lead text-muted mb-5\">La página que estás buscando no existe o fue movida.</p>"
"    <a href=\"/\" class=\"btn btn-primary btn-lg\">Volver al inicio</a>"
"  </div>"
""
"  <script>"
"    console.error(\"Error 404: Página no encontrada\");"
"  </script>",
        nullptr);
}


size_t ErrorView::Render(Action action) {
    size_t length = 0;

    switch (action) {
        case Action::GET:
            length = RenderGet();
            break;
        default: break;
    } 
    return length; 
}

size_t ErrorView::RenderGet() {
    
    _app->layout.RenderHeader("Simba - Error 404");
    _app->layout.SetInnerContent(&htmlBody);
    _app->layout.RenderFooter();
    _app->layout.SetEnd(&htmlBody);
    _app->layout.RenderHttpHeader(404, ContentType::TEXT_HTML);

    _app->UpdateRenderedContent();
    return _app->layout.TotalLength(); 

}
