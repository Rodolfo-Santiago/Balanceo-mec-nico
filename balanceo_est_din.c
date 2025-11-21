/*
UNIVERSIDAD NACIONAL AUTÓNOMA DE MÉXICO
Diseño de maquinaria
Balanceo mecánico
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#ifdef _WIN32
	#include <windows.h>
#endif

void presentacion_del_programa();
void inicio_del_programa(int*);

float* solicitar_datos(int);

double leer_double_seguro(const char*);
void limpiar_buffer_entrada();
int validar_dato(float, float, float);
void convertir_angulo(float*, int);

float* calcular_componentes(float*, int, float*);
float* calcular_componentes_de_mB(float*);
float* calcular_angulo_y_producto(float*);

void imprimir_resultado(float*, float*, char);
void reanudar_programa(int*);

int main()
{	
	#ifdef _WIN32
		SetConsoleOutputCP(CP_UTF8);
		SetConsoleCP(CP_UTF8);
	#endif

    int opcion;
    float* datos;
    float* component_mBrB, *resB;
    float* component_mArA, *resA;

    presentacion_del_programa();
    
    do {
        inicio_del_programa(&opcion);
    
        if (opcion == 1) {
            datos = solicitar_datos(2);
            
            datos = calcular_componentes(datos, 2, datos);
            
            datos = calcular_angulo_y_producto(datos);
            
            imprimir_resultado(datos, datos, ' ');
        } else {
            datos = solicitar_datos(3);
            
            component_mBrB = calcular_componentes_de_mB(datos);
            component_mArA = calcular_componentes(datos, 3, component_mBrB);
            
            resB = calcular_angulo_y_producto(component_mBrB);
            resA = calcular_angulo_y_producto(component_mArA);
            
            imprimir_resultado(resA, resB, 'A');
        }
        
        reanudar_programa(&opcion);
    } while (opcion == 1);
    
    
    return 0;
}

void presentacion_del_programa()
{
    printf("\nBALANCEO ESTÁTICO Y DINÁMICO\n"
	"\nEste programa realiza los cálculos para el balanceo estático "
	"y dinámico de un sistema mecánico móvil con dos o tres masas.\n"
	"Para el balanceo estático el programa toma como entrada la magnitud"
	" de dos masas ubicadas en un mismo plano y sus dos coordenadas polares,"
	" en metros y en grados, respectivamente.\n"
	"Para el balance dinámico el programa toma como entrada la magnitud de las"
	" tres masas dispuestas alrededor y a lo largo de un eje, y sus tres"
	" coordenadas cilíndricas, en metros y grados, considerando como origen"
	" del sistema coordenado a la intersección del eje con el plano de corrección A.\n"
	"También se toma como entrada la distancia del plano de correción A al plano de corrección B.\n");
}

void inicio_del_programa(int* opcion)
{
    printf("\n¿Qué cálculo deseas realizar?\n   [1] Balanceo estático con dos masas");
	printf("\n   [2] Balanceo dinámico con tres masas\n");
	
	do {
    	printf("\nIngresa el número de la opción que elegiste: ");
    	scanf("%d", opcion);
	} while (*opcion != 1 && *opcion != 2);

	limpiar_buffer_entrada();
}

float* solicitar_datos(int num_brazos)
{
    int i, j;
    float* datos = calloc(4 * num_brazos + 1, sizeof(float));
    
    for (i = 0, j = 1; j <= num_brazos; i += 4, j++)
    {
        do {
            printf("\nIngrese los datos del brazo %d:\n", j);
        	datos[i] = leer_double_seguro("Masa (kg): ");
        } while (validar_dato(datos[i], 0.001, 1000));
    	
    	do {
        	datos[i + 1] = leer_double_seguro("Radio (m)): ");
    	} while (validar_dato(datos[i + 1], 0.001, 100));
    	
    	do {
        	datos[i + 2] = leer_double_seguro("Ángulo (grados): ");
    	} while (validar_dato(datos[i + 2], 0, 360));
    	
    	convertir_angulo(datos + (i + 2), 0);
    	
    	if (num_brazos == 3)
    	{
        	do {
            	datos[i + 3] = leer_double_seguro("Distancia al plano A (m): ");
        	} while (validar_dato(datos[i + 3], 0.001, 100));
        	
        	if (j == 3) {
        	    do {
        	    printf("\nIngrese la distancia de la masa de balanceo B al plano A:\n");
            	datos[i + 4] = leer_double_seguro("Distancia (m): ");
        	    } while (validar_dato(datos[i + 4], 0.001, 100));
        	}
        	
    	}
    }
    
    return datos;
}

// Lector seguro para números de tipo double
double leer_double_seguro(const char* mensaje)
{	
	double numero;
	char* endptr;
	char buf[100];

	for (;;)
	{
		printf("\n%s", mensaje);

		// Leer cadena de caracteres de tamaño BUF del stdin
		if (fgets(buf, sizeof(buf), stdin) == NULL) {
			printf("\nError en entrada. Intente de nuevo.\n");
			continue;
		}

		// Revisar si después de leer la cadena sobró basura en stdin
		if (strchr(buf, '\n') == NULL)
			limpiar_buffer_entrada();

		// Convertir la cadena leída a número tipo double
		numero = strtod(buf, &endptr);

		// Limpiar espacios, tabs, o saltos de línea después del número (si los hay)
		while (isspace((unsigned char)*endptr))
			endptr++;

		// Después de todo, endptr debe apuntar al final de la cadena leída \0
		if (*endptr == '\0')
			return numero;

		printf("\nEntrada inválida. Debe ingresar un número.\n");
	}
}

// Limpiar los caracteres que están en el buffer de entrada (stdin)
void limpiar_buffer_entrada()
{
	int c;
	while ((c = getchar()) != '\n' && c != EOF)
		;
}

int validar_dato(float dato, float limite_inf, float limite_sup)
{
    if (dato < limite_inf || dato > limite_sup) {
        printf("\nError\nSolo puedes ingresar valores por encima de ");
        printf("%.4f, y menores a %.0f. ", limite_inf, limite_sup);
        printf("Vuelve a ingresar este dato nuevamente.\n\n");
        return 1;
    }
    return 0;
}

void convertir_angulo(float* angulo, int i)
{
	double pi = acos(-1.0);
    float constante[] = {pi, 180, pi};
    
    *angulo *= (constante[i] / constante[i + 1]);
}


float* calcular_componentes(float* datos, int num_brazos, float* mBrB)
{
    int i, j;
    float k;
    double pi = acos(-1.0);
    float* compo = (float*)calloc(2, sizeof(float));
    
    
    for (i = 0, j = 0; i < 2; j += 4)
    {
        if (i == 0) k = 2 * datos[j + 2];
        
        compo[i] -= datos[j] * datos[j + 1] * sin(k - datos[j + 2]);
        
        if (j == 8) compo[i] -= mBrB[i];
        
        if (j == 4 * (num_brazos - 1)) { i++; j = -4; k = pi / 2; }
    }
	
	free(datos);
	
	return compo;
}


float* calcular_componentes_de_mB(float* datos)
{
    int i, j;
    float k;
    double pi = acos(-1.0);
    float* componentes_B = (float*)calloc(2, sizeof(float));
    
    
    for (i = 0, j = 0; i < 2; j += 4)
    {
        if (i == 0) k = 2 * datos[j + 2];
        
        componentes_B[i] -= (datos[j] * datos[j + 1] * sin(k - datos[j + 2]) * datos[j + 3]) / datos[12];
        
        if (j == 8) { i++; j = -4; k = pi / 2; }
    }
	
	return componentes_B;
}

float* calcular_angulo_y_producto(float* res)
{
	double pi = acos(-1.0);
    float tol = 0.0001;
    float mbry = res[0];
    float mbrx = res[1];
    
	if (tol > mbry && mbry > -tol && tol > mbrx && mbrx > -tol) res[0] = 0;
	else if (mbry > 0 && mbrx == 0) res[0] = 90;
	else if (mbry < 0 && mbrx == 0) { res[0] = 270; }
	else if (mbry == 0 && mbrx < 0) { res[0] = 180; }

	if (mbry < 0 && mbrx > 0) { // En el cuadrante IV sumamos 2pi
	    res[0] = 2 * pi + atan(mbry / mbrx);
	    convertir_angulo(res, 1);
	}
	else if (mbrx < 0) // En el cuadrante II o III sumamos pi
	{
	    res[0] = pi + atan(mbry / mbrx);
	    convertir_angulo(res, 1);
	}
	else { // Cuadrante I 
	    res[0] = atan(mbry / mbrx);
	    convertir_angulo(res, 1);
	}

	res[1] = sqrt(pow(mbry, 2) + pow(mbrx, 2));
	
	return res;
}

void imprimir_resultado(float* res, float* res2, char letra)
{
    printf("\nPropiedades de la masa de balanceo %c", letra);
	printf("\n    Ángulo: %.2f°\n    Producto masa-radio: %.4f kg m\n", res[0], res[1]);
	
	if (letra == 'A') {
	    printf("\nPropiedades de la masa de balanceo B");
	    printf("\n    Ángulo: %.2f°\n    Producto masa-radio: %.4f kg m\n", res2[0], res2[1]);
	    free(res2);
	}
	
	free(res);
}

void reanudar_programa(int* opcion)
{
    printf("\n\n¿Qué acción deseas realizar ahora?\n   [1] Hacer otro cálculo");
	printf("\n   [2] Cerrar el programa\n");
	
	do {
    	printf("\nIngresa el número de la opción que elegiste: ");
    	scanf("%d", opcion);
	} while (*opcion != 1 && *opcion != 2);
}
