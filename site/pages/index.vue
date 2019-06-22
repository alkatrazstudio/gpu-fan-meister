<template>
    <v-layout row wrap>
        <v-flex sm3 xs12>
            <img-ex class="icon" src="../../assets/gpu-fan-meister.png" :aspect-ratio="1"/>
        </v-flex>
        <v-flex sm9 xs12>
            <h2 class="text-xs-center text-sm-left" :class="titleClass">GPU Fan Meister</h2>
            <h3 class="subheading font-italic text-xs-center text-sm-left">
                {{ $t('SUBHEADING') }}
            </h3>
        </v-flex>
        <v-flex md9 offset-md3 xs12>
            <div class="text-md-left text-sm-center text-xs-center">
                <v-btn color="green" @click.stop="dlgDownload = true" class="ml-0">
                    <v-icon left>mdi-download</v-icon>
                    {{ $t('BTN_DOWNLOAD') }}
                    <span class="version pl-1">({{ version }})</span>
                </v-btn>
                <v-btn :href="localeLink('doc')" color="blue" target="_blank">
                    <v-icon left>mdi-file-document-box-multiple-outline</v-icon>
                    {{ $t('BTN_DOCUMENTATION') }}
                </v-btn>
                <v-btn href="https://github.com/alkatrazstudio/gpu-fan-meister" target="_blank" color="blue-grey" rel="noreferrer">
                    <v-icon left>mdi-github-circle</v-icon>
                    GitHub
                </v-btn>
            </div>
            <div class="subheading available-on text-md-left text-sm-center text-xs-center">{{ $t('AVAILABLE_ON') }}</div>
        </v-flex>
        <v-flex md12 xs12>
            <v-carousel>
                <v-carousel-item v-for="(uri, i) in screens" :key="i">
                    <img-ex :aspect-ratio="2300/1700" :src="uri"/>
                </v-carousel-item>
            </v-carousel>
        </v-flex>
        <v-dialog v-model="dlgDownload" max-width="600">
            <v-card>
                <v-card-title class="headline">{{ sprintf($t('DOWNLOAD_TITLE'), 'GPU Fan Meister') }}</v-card-title>
                <v-card-text>
                    <v-data-table :headers="tableHeaders" :items="downloadInfo" hide-headers hide-actions>
                        <template slot="items" slot-scope="props">
                            <td>{{ props.item.label }}</td>
                            <td class="text-xs-right">
                                <a v-if="props.item.link" :href="props.item.link" target="_blank" rel="noreferrer">
                                    {{ props.item.text }}
                                </a>
                                <template v-else>
                                    {{ props.item.text }}
                                </template>
                            </td>
                        </template>
                    </v-data-table>
                </v-card-text>
                <v-card-actions>
                    <v-spacer></v-spacer>
                    <v-btn color="white" flat @click="dlgDownload = false">{{ $t('BTN_CLOSE') }}</v-btn>
                    <v-btn color="green" :href="downloadLink" target="_blank" rel="noreferrer" class="btn-download">{{ $t('BTN_DOWNLOAD') }}</v-btn>
                </v-card-actions>
            </v-card>
        </v-dialog>
    </v-layout>
</template>

<script>
import ImgEx from '~/components/imgex'
import * as prettyBytes from 'pretty-bytes'
import {RELEASE_SIZE, RELEASE_VERSION, RELEASE_DATE} from '~/data/release'

export default {
    data() {
        const versionStr = RELEASE_VERSION.join('.')
        return {
            dlgDownload: false,
            version: `v${versionStr}`,

            tableHeaders: [{
                text: 'Label'
            }, {
                text: 'Text'
            }]
        }
    },

    computed: {
        screens() {
            return ['curve', 'fanToggle', 'script', 'options']
                .map(s => `/img/screens/${this.$i18n.locale}/${s}.png`)
        },

        titleClass() {
            return ['xs', 'sm']
                .indexOf(this.$vuetify.breakpoint.name) != -1
                    ? 'display-2'
                    : 'display-3'
        },

        downloadInfo() {
            const dateOpts = {year: 'numeric', month: 'long', day: 'numeric'}
            const dateFmt = new Intl.DateTimeFormat(this.$i18n.locale, dateOpts)
            const dateObj = new Date(Date.UTC(RELEASE_DATE[0], RELEASE_DATE[1] - 1, RELEASE_DATE[2], 0, 0, 0))

            return [{
                label: 'OS',
                text: 'Linux'
            }, {
                label: 'FORMAT',
                text: 'AppImage',
                link: 'https://appimage.org'
            }, {
                label: 'SIZE',
                text: prettyBytes(RELEASE_SIZE, {locale: this.$i18n.locale})
            }, {
                label: 'VERSION',
                text: this.version
            }, {
                label: 'RELEASE_DATE',
                text: dateFmt.format(dateObj)
            }, {
                label: 'CHANGELOG',
                text: this.$t('DOWNLOAD_INFO_LINK'),
                link: 'https://github.com/alkatrazstudio/gpu-fan-meister/blob/master/CHANGELOG.rst'
            }, {
                label: 'LICENSE',
                text: 'GPLv3',
                link: 'https://www.gnu.org/licenses/gpl-3.0.html'
            }]
            .map(line => ({
                ...line,
                label: this.$t(`DOWNLOAD_INFO_LABEL_${line.label}`)
            }))
        },

        downloadLink() {
            return `https://github.com/alkatrazstudio/gpu-fan-meister/releases/download/${this.version}/GPU_Fan_Meister-x86_64.AppImage`
        }
    },

    methods: {
        sprintf(s, arg){
            return s.replace('%s', arg)
        },

        localeLink(s){
            return this.localePath('index').replace(/\/$/, '') + '/' + s
        }
    },

    components: {
        ImgEx
    },

    head() {
        return {
            meta: [{
                name: 'description',
                content: this.$t('META_DESCRIPTION')
            }]
        }
    }
}
</script>

<style scoped lang="scss">
.icon {
    width: 128px;
    margin: 0 auto;
    box-shadow: 0px 0px 3px 2px rgba(0,0,0,0.5);
    border-radius: 5px;

    & /deep/ .v-image__image {
        box-shadow: 0px 0px 10px 5px rgba(0,0,0,1) inset;
    }
}

.available-on {
    font-style: italic;
    opacity: 0.75;
}

.version {
    text-transform: lowercase;
}

.v-carousel {
    &,
    & /deep/ .v-window__container,
    & /deep/ .v-carousel__item {
        height: calc((1185px - 2 * 24px) / 2300 * 1700) !important;

        @media (max-width: 1263px) {
            height: calc((100vw - 2 * 24px) / 2300 * 1700) !important;
        }

        @media (max-width: 899px) {
            height: calc((100vw - 2 * 16px) / 2300 * 1700) !important;
        }
    }
}

.download-info {
    li {
        label {
            font-weight: bold;
            margin-right: 5px;
        }
    }
}
</style>
