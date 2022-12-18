
#include "App.h"
#include "Render.h"
#include "Textures.h"
#include "Map.h"
#include "Physics.h"

#include "Defs.h"
#include "Log.h"

#include <math.h>
#include "SDL_image/include/SDL_image.h"

#include "Fonts.h"
#include "Scene_Level1.h"

Map::Map() : Module(), mapLoaded(false)
{
    name.Create("map");
}

// Destructor
Map::~Map()
{

}

// Called before render is available
bool Map::Awake(pugi::xml_node& config)
{
    LOG("Loading Map Parser");
    bool ret = true;

    mapFileName = config.child("mapfile").attribute("path").as_string();
    mapFolder = config.child("mapfolder").attribute("path").as_string();

    return ret;
}

bool Map::CreateWalkabilityMap(int& width, int& height, uchar** buffer) const
{
    bool ret = false;
    ListItem<MapLayer*>* item;
    item = mapData.maplayers.start;

    for (item = mapData.maplayers.start; item != NULL; item = item->next)
    {
        MapLayer* layer = item->data;

        if (layer->properties.GetProperty("Navigation") == NULL || !layer->properties.GetProperty("Navigation")->value)
            continue;

        uchar* map = new uchar[layer->width * layer->height];
        memset(map, 1, layer->width * layer->height);

        for (int y = 0; y < mapData.height; ++y)
        {
            for (int x = 0; x < mapData.width; ++x)
            {
                int i = (y * layer->width) + x;

                int tileId = layer->Get(x, y);
                TileSet* tileset = (tileId > 0) ? GetTilesetFromTileId(tileId) : NULL;

                if (tileset != NULL)
                {
                    if (tileId == 655)
                    {
                        map[i] = 2;
                    }
                    else if (tileId == 656)
                    {
                        map[i] = 3;
                    }
                    else
                    {
                        map[i] = (tileId - tileset->firstgid) > 0 ? 0 : 1;
                    }
                }
            }
        }

        *buffer = map;
        width = mapData.width;
        height = mapData.height;
        ret = true;

        break;
    }

    return ret;
}

void Map::Draw()
{
    if(mapLoaded == false) return;

    ListItem<MapLayer*>* mapLayerItem;
    mapLayerItem = mapData.maplayers.start;

    while (mapLayerItem != NULL) 
    {

        if (mapLayerItem->data->properties.GetProperty("draw") != NULL && mapLayerItem->data->properties.GetProperty("draw")->value)
        {
            for (int x = 0; x < mapLayerItem->data->width; x++)
            {
                for (int y = 0; y < mapLayerItem->data->height; y++)
                {
                    int gid = mapLayerItem->data->Get(x, y);

                    TileSet* tileset = GetTilesetFromTileId(gid); // (!!) we are using always the first tileset in the list

                    SDL_Rect r = tileset->GetTileRect(gid);
                    iPoint pos = MapToScreen(x, y);

                    SDL_Texture* opacity_texture = tileset->texture;

                    SDL_SetTextureAlphaMod(opacity_texture, mapLayerItem->data->opacity * 255.0f);

                    app->render->DrawTexture(opacity_texture,
                        pos.x,
                        pos.y,
                        &r);
                }
            }
        }

        mapLayerItem = mapLayerItem->next;

    }
}

//Translates x, y coordinates from map positions to screen coordinates
iPoint Map::MapToScreen(int x, int y) const
{
    iPoint ret;

    ret.x = x * mapData.tileWidth;
    ret.y = y * mapData.tileHeight;

    return ret;
}

//Obtain map positions from screen coordinates
iPoint Map::ScreenToMap(int x, int y)
{
    iPoint ret(0, 0);

    if (mapData.type == MAPTYPE_ORTHOGONAL)
    {
        ret.x = x / mapData.tileWidth;
        ret.y = y / mapData.tileHeight;
    }
    else if (mapData.type == MAPTYPE_ISOMETRIC)
    {
        float halfWidth = mapData.tileWidth * 0.5f;
        float halfHeight = mapData.tileHeight * 0.5f;
        ret.x = int((x / halfWidth + y / halfHeight) / 2);
        ret.y = int((y / halfHeight - x / halfWidth) / 2);
    }
    else
    {
        LOG("Unknown map type");
        ret.x = x; ret.y = y;
    }

    return ret;
}

// Get relative Tile rectangle
SDL_Rect TileSet::GetTileRect(int gid) const
{
    SDL_Rect rect = { 0 };
    int relativeIndex = gid - firstgid;

    rect.w = tileWidth;
    rect.h = tileHeight;
    rect.x = margin + (tileWidth + spacing) * (relativeIndex % columns);
    rect.y = margin + (tileWidth + spacing) * (relativeIndex / columns);

    return rect;
}

TileSet* Map::GetTilesetFromTileId(int gid) const
{
    TileSet* set = NULL;

    ListItem<TileSet*>* item = mapData.tilesets.start;
    while (item != NULL)
    {
        set = item->data;
        if (gid < item->data->firstgid + item->data->tilecount)
        {
            break;
        }
        item = item->next;
    }

    return set;
}

// Called before quitting
bool Map::CleanUp()
{
    LOG("Unloading map");

	ListItem<TileSet*>* item;
	item = mapData.tilesets.start;

	while (item != NULL)
	{
		RELEASE(item->data);
		item = item->next;
	}
	mapData.tilesets.Clear();

    // Remove all layers
    ListItem<MapLayer*>* layerItem;
    layerItem = mapData.maplayers.start;

    while (layerItem != NULL)
    {
        RELEASE(layerItem->data);
        layerItem = layerItem->next;
    }

    return true;
}

// Load new map
bool Map::Load()
{
    bool ret = true;

    pugi::xml_document mapFileXML;
    pugi::xml_parse_result result = mapFileXML.load_file(mapFileName.GetString());

    if(result == NULL)
    {
        LOG("Could not load map xml file %s. pugi error: %s", mapFileName, result.description());
        ret = false;
    }

    if(ret == true)
    {
        ret = LoadMap(mapFileXML);
    }

    if (ret == true)
    {
        ret = LoadTileSet(mapFileXML);
    }

    if (ret == true)
    {
        ret = LoadAllLayers(mapFileXML.child("map"));
    }

    if (ret == true)
    {
        ret = LoadColliders(mapFileXML.child("map"));
    }

    if(ret == true)
    {
       
        LOG("Successfully parsed map XML file :%s", mapFileName.GetString());
        LOG("width : %d height : %d",mapData.width,mapData.height);
        LOG("tile_width : %d tile_height : %d",mapData.tileWidth, mapData.tileHeight);
        
        LOG("Tilesets----");

        ListItem<TileSet*>* tileset;
        tileset = mapData.tilesets.start;

        while (tileset != NULL) {
            LOG("name : %s firstgid : %d",tileset->data->name.GetString(), tileset->data->firstgid);
            LOG("tile width : %d tile height : %d", tileset->data->tileWidth, tileset->data->tileHeight);
            LOG("spacing : %d margin : %d", tileset->data->spacing, tileset->data->margin);
            tileset = tileset->next;
        }

        ListItem<MapLayer*>* mapLayer;
        mapLayer = mapData.maplayers.start;

        while (mapLayer != NULL) {
            LOG("id : %d name : %s", mapLayer->data->id, mapLayer->data->name.GetString());
            LOG("Layer width : %d Layer height : %d", mapLayer->data->width, mapLayer->data->height);
            mapLayer = mapLayer->next;
        }
    }

    if(mapFileXML) mapFileXML.reset();

    mapLoaded = ret;

    return ret;
}

bool Map::LoadMap(pugi::xml_node mapFile)
{
    bool ret = true;
    pugi::xml_node map = mapFile.child("map");

    if (map == NULL)
    {
        LOG("Error parsing map xml file: Cannot find 'map' tag.");
        ret = false;
    }
    else
    {
        //Load map general properties
        mapData.height = map.attribute("height").as_int();
        mapData.width = map.attribute("width").as_int();
        mapData.tileHeight = map.attribute("tileheight").as_int();
        mapData.tileWidth = map.attribute("tilewidth").as_int();
    }

    mapData.type = MAPTYPE_UNKNOWN;
    if ((SString)map.attribute("orientation").as_string() == "isometric")
    {
        mapData.type = MAPTYPE_ISOMETRIC;
    }
    if ((SString)map.attribute("orientation").as_string() == "orthogonal")
    {
        mapData.type = MAPTYPE_ORTHOGONAL;
    }

    return ret;
}

bool Map::LoadTileSet(pugi::xml_node mapFile){

    bool ret = true; 

    pugi::xml_node tileset;
    for (tileset = mapFile.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
    {
        TileSet* set = new TileSet();

        set->name = tileset.attribute("name").as_string();
        set->firstgid = tileset.attribute("firstgid").as_int();
        set->margin = tileset.attribute("margin").as_int();
        set->spacing = tileset.attribute("spacing").as_int();
        set->tileWidth = tileset.attribute("tilewidth").as_int();
        set->tileHeight = tileset.attribute("tileheight").as_int();
        set->columns = tileset.attribute("columns").as_int();
        set->tilecount = tileset.attribute("tilecount").as_int();

        SString tmp("%s%s", mapFolder.GetString(), tileset.child("image").attribute("source").as_string());
        set->texture = app->tex->Load(tmp.GetString());

        mapData.tilesets.Add(set);
    }

    return ret;
}

bool Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
    bool ret = true;

    //Load the attributes
    layer->id = node.attribute("id").as_int();
    layer->name = node.attribute("name").as_string();
    layer->width = node.attribute("width").as_int();
    layer->height = node.attribute("height").as_int();
    layer->opacity = node.attribute("opacity").as_float();

    Properties* ps = new Properties();
    LoadProperties(node.child("properties"), layer->properties);

    //Reserve the memory for the data 
    layer->data = new uint[layer->width * layer->height];
    memset(layer->data, 0, layer->width * layer->height);

    //Iterate over all the tiles and assign the values
    pugi::xml_node tile;
    int i = 0;
    for (tile = node.child("data").child("tile"); tile && ret; tile = tile.next_sibling("tile"))
    {
        layer->data[i] = tile.attribute("gid").as_int();
        i++;
    }

    return ret;
}

bool Map::LoadAllLayers(pugi::xml_node mapNode) {
    bool ret = true;

    for (pugi::xml_node layerNode = mapNode.child("layer"); layerNode && ret; layerNode = layerNode.next_sibling("layer"))
    {
        //Load the layer
        MapLayer* mapLayer = new MapLayer();
        ret = LoadLayer(layerNode, mapLayer);

        //add the layer to the map
        mapData.maplayers.Add(mapLayer);
    }

    return ret;
}

bool Map::LoadColliders(pugi::xml_node mapFile)
{
    bool ret = true;

    //Iterate over all the colliders and assign the values
    //pugi::xml_node parent = mapFile.child("objectgroup");
    for (pugi::xml_node parent = mapFile.child("objectgroup"); parent && ret; parent = parent.next_sibling("objectgroup"))
    {
        if ((SString)parent.attribute("name").as_string() == "Colliders")
        {
            for (pugi::xml_node collider = parent.child("object"); collider && ret; collider = collider.next_sibling("object"))
            {
                PhysBody* box = app->physics->CreateRectangle(collider.attribute("x").as_int() + collider.attribute("width").as_int() / 2,
                    collider.attribute("y").as_int() + collider.attribute("height").as_int() / 2,
                    collider.attribute("width").as_int(),
                    collider.attribute("height").as_int(), STATIC);
                box->ctype = ColliderType::PLATFORM;
                app->scene->boxes.Add(box);
            }
        }
        if ((SString)parent.attribute("name").as_string() == "Level_Walls")
        {
            for (pugi::xml_node collider = parent.child("object"); collider && ret; collider = collider.next_sibling("object"))
            {
                PhysBody* box = app->physics->CreateRectangle(collider.attribute("x").as_int() + collider.attribute("width").as_int() / 2,
                    collider.attribute("y").as_int() + collider.attribute("height").as_int() / 2,
                    collider.attribute("width").as_int(),
                    collider.attribute("height").as_int(), STATIC);
                box->ctype = ColliderType::LIMIT;
                app->scene->boxes.Add(box);
            }
        }
        else if ((SString)parent.attribute("name").as_string() == "Triggers")
        {
            for (pugi::xml_node collider = parent.child("object"); collider && ret; collider = collider.next_sibling("object"))
            {
                PhysBody* box = app->physics->CreateRectangleSensor(collider.attribute("x").as_int() + collider.attribute("width").as_int() / 2,
                    collider.attribute("y").as_int() + collider.attribute("height").as_int() / 2,
                    collider.attribute("width").as_int(),
                    collider.attribute("height").as_int(), STATIC);
                box->ctype = ColliderType::GOAL;
                app->scene->boxes.Add(box);
            }
        }
        else if ((SString)parent.attribute("name").as_string() == "Jump_Triggers")
        {
            for (pugi::xml_node collider = parent.child("object"); collider && ret; collider = collider.next_sibling("object"))
            {
                PhysBody* box = app->physics->CreateRectangleSensor(collider.attribute("x").as_int() + collider.attribute("width").as_int() / 2,
                    collider.attribute("y").as_int() + collider.attribute("height").as_int() / 2,
                    collider.attribute("width").as_int(),
                    collider.attribute("height").as_int(), STATIC);

                box->impulse.x = collider.child("properties").child("property").attribute("value").as_int();
                box->impulse.y = collider.child("properties").child("property").next_sibling().attribute("value").as_int();

                box->ctype = ColliderType::JUMPTRIGGER;
                app->scene->boxes.Add(box);
            }
        }
        else if ((SString)parent.attribute("name").as_string() == "Spikes")
        {
            for (pugi::xml_node collider = parent.child("object"); collider && ret; collider = collider.next_sibling("object"))
            {
                PhysBody* box = app->physics->CreateRectangle(collider.attribute("x").as_int() + collider.attribute("width").as_int() / 2,
                    collider.attribute("y").as_int() + collider.attribute("height").as_int() / 2,
                    collider.attribute("width").as_int(),
                    collider.attribute("height").as_int(), STATIC);
                box->ctype = ColliderType::SAW;
                app->scene->boxes.Add(box);
            }
        }
        else if ((SString)parent.attribute("name").as_string() == "Saws")
        {
            for (pugi::xml_node collider = parent.child("object"); collider && ret; collider = collider.next_sibling("object"))
            {
                PhysBody* saw = app->physics->CreateCircle(collider.attribute("x").as_int() + collider.attribute("width").as_int() / 2,
                    collider.attribute("y").as_int() + collider.attribute("height").as_int() / 2,
                    collider.attribute("width").as_int()/2, STATIC);
                saw->ctype = ColliderType::SAW;
                app->scene->saws.Add(saw);
            }
        }
    }

    return ret;
}

bool Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
    bool ret = true;

    for (pugi::xml_node propertyNode = node.child("property"); propertyNode && ret; propertyNode = propertyNode.next_sibling("property"))
    {
        Properties::Property* p = new Properties::Property();
        p->name = propertyNode.attribute("name").as_string();
        p->type = propertyNode.attribute("type").as_string();
        p->value = propertyNode.attribute("value").as_bool();

        properties.list.Add(p);
    }

    return ret;
}

Properties::Property* Properties::GetProperty(const char* name)
{
    ListItem<Property*>* item = list.start;
    Property* p = NULL;

    while (item)
    {
        if (item->data->name == name) {
            p = item->data;
            break;
        }
        item = item->next;
    }

    return p;
}
