import QtQuick 2.15
import QtQuick.Window 2.15
import QtPositioning 5.15
import QtLocation 5.15

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    Plugin {
            id: mapPlugin
            name: "osm" // "mapboxgl", "esri", ...
            PluginParameter { name: "osm.useragent"; value: "My great Qt OSM application" }
            PluginParameter { name: "osm.mapping.host"; value: "http://osm.tile.server.address/" }
            PluginParameter { name: "osm.mapping.copyright"; value: "All mine" }
            PluginParameter { name: "osm.routing.host"; value: "http://osrm.server.address/viaroute" }
            PluginParameter { name: "osm.geocoding.host"; value: "http://geocoding.server.address" }
        }

        Map {
            anchors.fill: parent
            plugin: mapPlugin
            center: QtPositioning.coordinate(59.91, 10.75) // Oslo
            zoomLevel: 14
        }
}
