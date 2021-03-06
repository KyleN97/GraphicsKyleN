// FRAGMENT SHADER 
#version 410
 in vec2 fTexCoord;
 out vec4 fragColour; 
 uniform sampler2D target;
 uniform bool distort;
 uniform bool blur;
 uniform bool greyScale;
 // just sample the target and return the colour
 vec4 Distort()
 { 
	vec2 mid = vec2(0.5f);
	float distanceFromCentre = distance(fTexCoord, mid); 
	vec2 normalizedCoord = normalize(fTexCoord - mid); 
	float bias = distanceFromCentre + sin(distanceFromCentre * 15) * 0.02f;
	vec2 newCoord = mid + bias * normalizedCoord;
	return texture(target, newCoord);
 } 
 vec4 BoxBlur()
 {
	vec2 texel = 1.0f / textureSize(target, 0).xy; 
	// 9-tap box kernel
	vec4 colour = texture(target, fTexCoord); 
	colour += texture(target, fTexCoord + vec2(-texel.x, texel.y));
	colour += texture(target, fTexCoord + vec2(-texel.x, 0));
	colour += texture(target, fTexCoord + vec2(-texel.x, -texel.y)); 
	colour += texture(target, fTexCoord + vec2(0, texel.y));
	colour += texture(target, fTexCoord + vec2(0, -texel.y)); 
	colour += texture(target, fTexCoord + vec2(texel.x, texel.y)); 
	colour += texture(target, fTexCoord + vec2(texel.x, 0)); 
	colour += texture(target, fTexCoord + vec2(texel.x, -texel.y)); 
	return colour / 9;
 }
 vec4 GreyScale()
 {
	vec4 depth;
	float d = texture(target,fTexCoord).x;
	depth = vec4(d,d,d,1);
	return depth;
 }
 void main() 
 { 
	if(distort == true){
		fragColour = Distort();
	}
	else if(blur == true){
		fragColour = BoxBlur();
	}
	else if(greyScale == true){
		fragColour = GreyScale();
	}
 }