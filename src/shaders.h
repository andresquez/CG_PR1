#include "glm/glm.hpp"
#include "uniform.h"
#include "fragment.h"
#include "color.h"
#include "vertex.h"
#include "FastNoiseLite.h"
#include "framebuffer.h"
#include <cstdlib> 
#pragma once
#include <cstdlib>  // Necesario para usar rand()


enum Shaders{
    sol,
    tierra,
    neptuno,
    marte,
    jupiter,
    mercurio,
    nave
};

struct Planeta{
    Uniform uniform;
    std::vector<Vertex>* vertex;
    Shaders shader;
};

Vertex vertexShader(const Vertex& vertex, const Uniform& uniform) {
    glm::vec4 transformedVertex = uniform.viewport * uniform.projection * uniform.view * uniform.model * glm::vec4(vertex.position, 1.0f);
    glm::vec3 vertexRedux;
    vertexRedux.x = transformedVertex.x / transformedVertex.w;
    vertexRedux.y = transformedVertex.y / transformedVertex.w;
    vertexRedux.z = transformedVertex.z / transformedVertex.w;
    Color fragmentColor(255, 0, 0, 255);
    glm::vec3 normal = glm::normalize(glm::mat3(uniform.model) * vertex.normal);
    Fragment fragment;
    fragment.position = glm::ivec2(transformedVertex.x, transformedVertex.y);
    fragment.color = fragmentColor;
    return Vertex {vertexRedux, normal, vertex.tex, vertex.position};
}


Color shaderPlanet4(Fragment& fragment) {
    Color groundColor(34, 139, 34);
    Color oceanColor(0, 0, 139);
    Color cloudColor(255, 255, 255);

    glm::vec2 uv = glm::vec2(fragment.original.x, fragment.original.y);

    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    float ox = 1100.0f;
    float oy = 2800.0f;
    float zoom = 80.0f;

    float noiseValue = noiseGenerator.GetNoise((uv.x + ox) * zoom, (uv.y + oy) * zoom);

    FastNoiseLite noiseGenerator2;
    noiseGenerator2.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    float ox2 = 3000.0f;
    float oy2 = 5000.0f;
    float zoom2 = 450.0f;

    float noiseValue2 = noiseGenerator2.GetNoise((uv.x + ox2) * zoom2, (uv.y + oy2) * zoom2);

    noiseValue = (noiseValue + noiseValue2) * 1.0f;

    // Colores y umbrales para los patrones
    float patternThreshold1 = 0.2f;
    float patternThreshold2 = 0.6f;
    float patternThreshold3 = 0.9f;

    Color patternColor1(0, 242, 255); // #00f2ff
    Color patternColor2(173, 255, 150); // #a8ff96
    Color patternColor3(255, 255, 255); // #ffffff

    Color tmpColor = (noiseValue < patternThreshold1) ? patternColor1 : ((noiseValue < patternThreshold2) ? patternColor2 : patternColor3);

    float oxc = 5800.0f;
    float oyc = 6500.0f;
    float zoomc = 1000.0f;

    float noiseValueC = noiseGenerator.GetNoise((uv.x + oxc) * zoomc, (uv.y + oyc) * zoomc);

    if (noiseValueC > 0.7f) {
        tmpColor = cloudColor;
    }

    // Aplica la intensidad y la profundidad del fragmento
    float intensity = 1.2;
    fragment.color = tmpColor * fragment.z * intensity;

    return fragment.color;
}


Color shaderStar(Fragment& fragment) {
    // Base color for the star (cyan)
    Color baseColorCyan(0, 255, 255);

    // Coefficient of blending between the base color and noise
    float mixFactor = 0.001; // Ajusta según tus preferencias

    // Distance from the center for the gradient
    float gradientFactor = 4.0 - length(fragment.original);

    // Intensity to adjust brightness
    float intensity = 2.0; // Ajusta según tus preferencias

    // Blend between the base color and noise
    Color starColor = baseColorCyan * (1.0f - mixFactor) * gradientFactor * intensity;

    // Get UV coordinates of the fragment
    float uvX = fragment.original.x;
    float uvY = fragment.original.y;

    // Parameters for noise
    float noiseScale = 0.02; // Ajusta según tus preferencias

    // Generate noise for the star texture
    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    // Combine the two levels of noise
    float noiseValue = (noiseGenerator.GetNoise((uvX + 500.0f) * 500.0f, (uvY + 3000.0f) * 500.0f) +
                        noiseGenerator.GetNoise((uvX + 300.0f) * 500.0f, (uvY + 5000.0f) * 500.0f) * 0.2f) * 0.05f;

    // Blend between the base color and noise based on the noise value
    Color finalColor = starColor + (Color(1.0f, 1.0f, 1.0f) * noiseValue); // Ensure '1.0f' and correct data types

    // Apply intensity and the depth of the fragment
    fragment.color = finalColor * fragment.z * intensity;

    return fragment.color;
}

Color shaderPlanet1(Fragment& fragment) {
    // Color base del planeta (azul verdoso)
    Color planetColor(0, 150, 100);

    // Distancia desde el centro para el gradiente
    float distanceFromCenter = length(fragment.original);
    float gradientFactor = 1.0 - distanceFromCenter;

    // Intensidad para ajustar el brillo
    float intensity = 1.5;

    // Aplica el gradiente, la intensidad y el color base mezclado
    Color planetSurfaceColor = planetColor * gradientFactor * intensity;

    // Obtén las coordenadas UV del fragmento
    float uvX = fragment.original.x;
    float uvY = fragment.original.y;

    // Parámetros para el ruido
    float noiseScale = 0.2; // Ajusta según tus preferencias

    // Genera ruido para la textura del planeta
    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    float noiseValue = noiseGenerator.GetNoise((uvX + 5000.0) * 300.0, (uvY + 2000.0) * 300.0) * noiseScale;
    float noiseValue2 = noiseGenerator.GetNoise((uvX + 3000.0) * 300.0, (uvY + 5000.0) * 300.0) * (noiseScale * 0.5);

    // Combinación de los dos niveles de ruido
    noiseValue = (noiseValue + noiseValue2) * 0.5; // Ajusta según tus preferencias

    // Mezcla entre el color base y el ruido según el valor de ruido
    Color finalColor = planetSurfaceColor * (1.0 - noiseValue) + planetColor * noiseValue;

    // Aplica la intensidad y la profundidad del fragmento
    fragment.color = finalColor * fragment.z * intensity;

    return fragment.color;
}

Color shaderPlanet2(Fragment& fragment) {
    // Intensidad de las líneas
    float lineIntensity = 1.5; // Ajusta según tus preferencias

    // Número de líneas
    int numLines = 5; // Ajusta según tus preferencias

    // Color del planeta (blanco)
    Color planetColor(255, 255, 255);
    
    // Tono de los detalles (morado)
    Color detailColor(128, 0, 128);

    // Calcula la posición vertical normalizada en el rango [0, 1]
    float normalizedY = (fragment.original.y + 1.0) * 1.0;

    // Calcula el patrón de líneas horizontales
    float linePattern = glm::fract(normalizedY * numLines);

    // Aplica el efecto de líneas difuminadas
    float blur = 1.0 - abs(linePattern - 0.5) * 1.5;
    blur = pow(blur, lineIntensity);

    // Combina el color del planeta con el efecto de líneas difuminadas
    Color finalColor = planetColor * blur + detailColor * (1.0 - blur);

    fragment.color = finalColor;

    return fragment.color;
}

Color shaderPlanet3(Fragment& fragment) {
    // Color base del planeta (blanco)
    Color baseColorWhite(255, 255, 255);

    // Configuración de ruido para los patrones de puntos
    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    float noiseScale = 200.0f; // Ajusta la escala del ruido para los patrones
    float noiseValue = noiseGenerator.GetNoise(fragment.original.x * noiseScale, fragment.original.y * noiseScale, fragment.original.z * noiseScale);

    // Calcula el color final mezclando el color base con los patrones de puntos
    Color finalColor = baseColorWhite;

    // Define umbrales para las capas de patrones
    float patternThreshold1 = 0.1f; // Umbral para el primer patrón
    float patternThreshold2 = 0.5f; // Umbral para el segundo patrón
    float patternThreshold3 = 0.9f; // Umbral para el tercer patrón

    // Asigna colores a las capas de patrones (amarillo, magenta y cian)
    Color patternColor1(255, 255, 0); // Amarillo
    Color patternColor2(255, 0, 255); // Magenta
    Color patternColor3(0, 255, 255); // Cian

    // Aplica los colores de las capas de patrones según el valor de ruido
    if (noiseValue < patternThreshold1) {
        finalColor = MixColors(finalColor, patternColor1, noiseValue);
    } else if (noiseValue < patternThreshold2) {
        finalColor = MixColors(finalColor, patternColor2, noiseValue);
    } else if (noiseValue < patternThreshold3) {
        finalColor = MixColors(finalColor, patternColor3, noiseValue);
    }

    // Aplica la intensidad y la profundidad del fragmento
    float intensity = 1.2; // Ajusta según tus preferencias
    fragment.color = finalColor * fragment.z * intensity;

    return fragment.color;
}



Color shaderNave(Fragment& fragment) {
    // Base color for the star (cyan)
    Color baseColorCyan(0, 255, 0);

    // Coefficient of blending between the base color and noise
    float mixFactor = 0.1; // Ajusta según tus preferencias

    // Distance from the center for the gradient
    float gradientFactor = 4.0 - length(fragment.original);

    // Intensity to adjust brightness
    float intensity = 1; // Ajusta según tus preferencias

    // Blend between the base color and noise
    Color starColor = baseColorCyan * (1.0f - mixFactor) * gradientFactor * intensity;

    // Get UV coordinates of the fragment
    float uvX = fragment.original.x;
    float uvY = fragment.original.y;

    // Parameters for noise
    float noiseScale = 02.2; // Ajusta según tus preferencias

    // Generate noise for the star texture
    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    // Combine the two levels of noise
    float noiseValue = (noiseGenerator.GetNoise((uvX + 500.0f) * 500.0f, (uvY + 3000.0f) * 500.0f) +
                        noiseGenerator.GetNoise((uvX + 300.0f) * 500.0f, (uvY + 5000.0f) * 500.0f) * 0.2f) * 0.05f;

    // Blend between the base color and noise based on the noise value
    Color finalColor = starColor + (Color(1.5f, 1.0f, 1.0f) * noiseValue); // Ensure '1.0f' and correct data types

    // Apply intensity and the depth of the fragment
    fragment.color = finalColor * fragment.z * intensity;

    return fragment.color;
}

